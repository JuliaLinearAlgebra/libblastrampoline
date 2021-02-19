using Libdl, Test, OpenBLAS_jll, OpenBLAS32_jll
include("utils.jl")

function lbt_forward(handle, path; clear::Bool = false, verbose::Bool = false)
    ccall(dlsym(handle, :lbt_forward), Cint, (Cstring, Cint, Cint), path, clear ? 1 : 0, verbose ? 1 : 0)
end

# Keep these in sync with `src/libblastrampoline_internal.h`
struct lbt_library_info_t
    libname::Cstring
    handle::Ptr{Cvoid}
    suffix::Cstring
    interface::Int32
    f2c::Int32
end
struct LBTLibraryInfo
    libname::String
    handle::Ptr{Cvoid}
    suffix::String
    interface::Int32
    f2c::Int32

    LBTLibraryInfo(x::lbt_library_info_t) = new(unsafe_string(x.libname), x.handle, unsafe_string(x.suffix), x.interface, x.f2c)
end
const LBT_INTERFACE_LP64 = 32
const LBT_INTERFACE_ILP64 = 64
const LBT_F2C_PLAIN = 0

struct lbt_config_t
    loaded_libs::Ptr{Ptr{lbt_library_info_t}}
    build_flags::UInt32
end
const LBT_BUILDFLAGS_F2C_CAPABLE = 0x02

function lbt_get_config(handle)
    return unsafe_load(ccall(dlsym(handle, :lbt_get_config), Ptr{lbt_config_t}, ()))
end

function unpack_loaded_libraries(config::lbt_config_t)
    libs = LBTLibraryInfo[]
    idx = 1
    lib_ptr = unsafe_load(config.loaded_libs, idx)
    while lib_ptr != C_NULL
        push!(libs, LBTLibraryInfo(unsafe_load(lib_ptr)))

        idx += 1
        lib_ptr = unsafe_load(config.loaded_libs, idx)
    end
    return libs
end

@testset "Direct usage" begin
    lbt_prefix = get_blastrampoline_dir()
    lbt_handle = dlopen("$(lbt_prefix)/$(binlib)/libblastrampoline.$(shlib_ext)", RTLD_GLOBAL | RTLD_DEEPBIND)
    @test lbt_handle != C_NULL

    # Get immediate config, ensure that nothing is loaded
    config = lbt_get_config(lbt_handle)
    @test isempty(unpack_loaded_libraries(config))

    # Load OpenBLAS and OpenBLAS32_jll and then OpenBLAS_jll again
    lbt_forward(lbt_handle, OpenBLAS_jll.libopenblas_path; clear=true)
    lbt_forward(lbt_handle, OpenBLAS32_jll.libopenblas_path)
    lbt_forward(lbt_handle, OpenBLAS_jll.libopenblas_path)

    # Get config
    config = lbt_get_config(lbt_handle)

    # If we're x86_64, ensure LBT thinks it's f2c-adapter capable
    if Sys.ARCH == :x86_64
        @test (config.build_flags & LBT_BUILDFLAGS_F2C_CAPABLE) != 0
    end

    # Walk the libraries and check we have two
    libs = unpack_loaded_libraries(config)
    @test length(libs) == 2

    # First check OpenBLAS_jll which may or may not be ILP64
    @test libs[1].libname == OpenBLAS_jll.libopenblas_path
    if Sys.WORD_SIZE == 64 && Sys.ARCH != :aarch64
        @test libs[1].suffix == "64_"
        @test libs[1].interface == LBT_INTERFACE_ILP64
    else
        @test libs[1].suffix == ""
        @test libs[1].interface == LBT_INTERFACE_LP64
    end
    @test libs[1].f2c == LBT_F2C_PLAIN

    # Next check OpenBLAS32_jll which is always LP64
    @test libs[2].libname == OpenBLAS32_jll.libopenblas_path
    @test libs[2].suffix == ""
    @test libs[2].interface == LBT_INTERFACE_LP64
    @test libs[2].f2c == LBT_F2C_PLAIN

    # Load OpenBLAS32_jll again, but this time clearing it and ensure the config gets cleared too
    lbt_forward(lbt_handle, OpenBLAS32_jll.libopenblas_path; clear=true)
    config = lbt_get_config(lbt_handle)
    libs = unpack_loaded_libraries(config)
    @test length(libs) == 1
    @test libs[1].libname == OpenBLAS32_jll.libopenblas_path
    @test libs[1].suffix == ""
    @test libs[1].interface == LBT_INTERFACE_LP64
    @test libs[1].f2c == LBT_F2C_PLAIN
end

using Libdl, Test, OpenBLAS_jll, OpenBLAS32_jll, MKL_jll

include("utils.jl")

function unpack_loaded_libraries(config::lbt_config_t)
    libs = LBTLibraryInfo[]
    idx = 1
    lib_ptr = unsafe_load(config.loaded_libs, idx)
    while lib_ptr != C_NULL
        push!(libs, LBTLibraryInfo(unsafe_load(lib_ptr), config.num_exported_symbols))

        idx += 1
        lib_ptr = unsafe_load(config.loaded_libs, idx)
    end
    return libs
end

function find_symbol_offset(config::lbt_config_t, symbol::String)
    for sym_idx in 1:config.num_exported_symbols
        if unsafe_string(unsafe_load(config.exported_symbols, sym_idx)) == symbol
            return UInt32(sym_idx - 1)
        end
    end
    return nothing
end

function bitfield_get(field::Vector{UInt8}, symbol_idx::UInt32)
    return field[div(symbol_idx,8)+1] & (UInt8(0x01) << (symbol_idx%8))
end

lbt_prefix = get_blastrampoline_dir()
lbt_handle = dlopen("$(lbt_prefix)/$(binlib)/libblastrampoline.$(shlib_ext)", RTLD_GLOBAL | RTLD_DEEPBIND)

@testset "Config" begin
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

    # Check to make sure that `dgemm_` is part of the exported symbols:
    dgemm_idx = find_symbol_offset(config, "dgemm_")
    @test dgemm_idx !== nothing

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
    @test bitfield_get(libs[1].active_forwards, dgemm_idx) != 0

    # Next check OpenBLAS32_jll which is always LP64
    @test libs[2].libname == OpenBLAS32_jll.libopenblas_path
    @test libs[2].suffix == ""
    @test libs[2].interface == LBT_INTERFACE_LP64
    @test libs[2].f2c == LBT_F2C_PLAIN

    # If OpenBLAS32 and OpenBLAS are the same interface (e.g. i686)
    # then libs[2].active_forwards should be all zero!
    if libs[1].interface == libs[2].interface
        @test bitfield_get(libs[2].active_forwards, dgemm_idx) == 0
    else
        @test bitfield_get(libs[2].active_forwards, dgemm_idx) != 0
    end

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

@testset "get/set threads" begin
    lbt_forward(lbt_handle, OpenBLAS32_jll.libopenblas_path; clear=true)

    # get/set threads
    nthreads = ccall(dlsym(OpenBLAS32_jll.libopenblas_handle, :openblas_get_num_threads), Cint, ())
    @test lbt_get_num_threads(lbt_handle) == nthreads
    nthreads = div(nthreads, 2)
    lbt_set_num_threads(lbt_handle, nthreads)
    @test ccall(dlsym(OpenBLAS32_jll.libopenblas_handle, :openblas_get_num_threads), Cint, ()) == nthreads
    @test lbt_get_num_threads(lbt_handle) == nthreads

    # If we're on a 64-bit system, load OpenBLAS_jll in and cause a mismatch in the threading
    if Sys.WORD_SIZE == 64 && Sys.ARCH != :aarch64
        lbt_forward(lbt_handle, OpenBLAS_jll.libopenblas_path)

        lbt_set_num_threads(lbt_handle, 1)
        @test lbt_get_num_threads(lbt_handle) == 1
        @test ccall(dlsym(OpenBLAS32_jll.libopenblas_handle, :openblas_get_num_threads), Cint, ()) == 1

        ccall(dlsym(OpenBLAS32_jll.libopenblas_handle, :openblas_set_num_threads), Cvoid, (Cint,), 2)
        @test lbt_get_num_threads(lbt_handle) == 2
        lbt_set_num_threads(lbt_handle, 1)
        @test lbt_get_num_threads(lbt_handle) == 1
    end
end

slamch_args = []
function record_slamch_args(str::Cstring)
    push!(slamch_args, unsafe_string(str))
    return 13.37f0
end

# This "default function" will keep track of everyone who tries to call an uninitialized BLAS function
stacktraces = []
function default_capture_stacktrace()
    push!(stacktraces, stacktrace(true))
    return nothing
end

@testset "footgun API" begin
    # Load OpenBLAS32
    lbt_forward(lbt_handle, OpenBLAS32_jll.libopenblas_path; clear=true)

    # Test that we can get the `dgemm_` symbol address, and that it is what we expect
    slamch_32 = dlsym(OpenBLAS32_jll.libopenblas_handle, :slamch_)
    @test slamch_32 != C_NULL
    @test lbt_get_forward(lbt_handle, "slamch_", LBT_INTERFACE_LP64) == slamch_32

    # Ensure that the libs show that `slamch_` is forwarded by this library:
    config = lbt_get_config(lbt_handle)
    libs = unpack_loaded_libraries(config)
    @test length(libs) == 1

    slamch_idx = find_symbol_offset(config, "slamch_")
    @test slamch_idx !== nothing
    @test bitfield_get(libs[1].active_forwards, slamch_idx) != 0
    orig_forwards = copy(libs[1].active_forwards)

    # Now, test that we can muck this up
    my_slamch = @cfunction(record_slamch_args, Float32, (Cstring,))
    @test lbt_set_forward(lbt_handle, "slamch_", my_slamch, LBT_INTERFACE_LP64) == 0
    @test lbt_get_forward(lbt_handle, "slamch_", LBT_INTERFACE_LP64) == my_slamch

    config = lbt_get_config(lbt_handle)
    libs = unpack_loaded_libraries(config)
    @test bitfield_get(libs[1].active_forwards, slamch_idx) == 0

    # Ensure that we actually overrode the symbol
    @test ccall(dlsym(lbt_handle, "slamch_"), Float32, (Cstring,), "test") == 13.37f0
    @test slamch_args == ["test"]

    # Override the default function to keep track of people who try to call uninitialized BLAS functions
    @test lbt_get_default_func(lbt_handle) != C_NULL
    my_default_func = @cfunction(default_capture_stacktrace, Cvoid, ())
    lbt_set_default_func(lbt_handle, my_default_func)
    @test lbt_get_default_func(lbt_handle) == my_default_func

    # Now, set `slamch_64_` to it
    @test lbt_set_forward(lbt_handle, "slamch_", C_NULL, LBT_INTERFACE_ILP64) == 0
    ccall(dlsym(lbt_handle, "slamch_64_"), Float32, (Cstring,), "this will call the default function")
    @test length(stacktraces) == 1
    self_traces = filter(entry -> string(entry.file) == @__FILE__, stacktraces[1])
    @test length(self_traces) == 3
end

if MKL_jll.is_available() && Sys.WORD_SIZE == 64
    # Since MKL v2022, we can explicitly link against ILP64-suffixed symbols
    @testset "MKL v2022 ILP64 loading" begin
        # Load the ILP64 interface library.  Remember, you must load the `core`
        # and a `threading` library first, with `RTLD_LAZY` for this to work!
        lbt_forward(lbt_handle, libmkl_rt; clear=true, suffix_hint = "64")

        # Test that we have only one library loaded
        config = lbt_get_config(lbt_handle)
        libs = unpack_loaded_libraries(config)
        @test length(libs) == 1

        # Test that it's MKL and it's correctly identified
        @test libs[1].libname == MKL_jll.libmkl_rt_path
        @test libs[1].interface == LBT_INTERFACE_ILP64

        # Test that `dgemm` forwards to `dgemm_` within the MKL binary
        mkl_dgemm = dlsym(MKL_jll.libmkl_rt_handle, :dgemm_64)
        @test lbt_get_forward(lbt_handle, "dgemm_", LBT_INTERFACE_ILP64) == mkl_dgemm
    end

    @testset "MKL v2022 dual-interface loading" begin
        # Also test that we can load both ILP64 and LP64 at the same time!
        lbt_forward(lbt_handle, libmkl_rt; clear=true, suffix_hint = "64")
        lbt_forward(lbt_handle, libmkl_rt; suffix_hint = "")

        # Test that we have both libraries loaded
        config = lbt_get_config(lbt_handle)
        libs = unpack_loaded_libraries(config)
        @test length(libs) == 2

        # Test that it's MKL and it's correctly identified
        sort!(libs; by = l -> l.libname)
        @test libs[1].libname == libmkl_rt
        @test libs[1].interface == LBT_INTERFACE_ILP64
        @test libs[2].libname == libmkl_rt
        @test libs[2].interface == LBT_INTERFACE_LP64
    end
end

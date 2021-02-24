if Sys.iswindows()
    LIBPATH_env = "PATH"
    LIBPATH_default = ""
    pathsep = ';'
    binlib = "bin"
    shlib_ext = "dll"
elseif Sys.isapple()
    LIBPATH_env = "DYLD_FALLBACK_LIBRARY_PATH"
    LIBPATH_default = "~/lib:/usr/local/lib:/lib:/usr/lib"
    pathsep = ':'
    binlib = "lib"
    shlib_ext = "dylib"
else
    LIBPATH_env = "LD_LIBRARY_PATH"
    LIBPATH_default = ""
    pathsep = ':'
    binlib = "lib"
    shlib_ext = "so"
end

function append_libpath(paths::Vector{<:AbstractString})
    return join(vcat(paths..., get(ENV, LIBPATH_env, String[])), pathsep)
end

function capture_output(cmd::Cmd; verbose::Bool = false)
    out_pipe = Pipe()
    if verbose
        ld_env = filter(e -> startswith(e, "LBT_") || startswith(e, "LD_") || startswith(e, "DYLD_"), something(cmd.env, String[]))
        @info("Running $(basename(cmd.exec[1]))", ld_env)
    end
    p = run(pipeline(ignorestatus(cmd), stdout=out_pipe, stderr=out_pipe), wait=false)
    close(out_pipe.in)
    output = @async read(out_pipe, String)
    wait(p)
    return fetch(output)
end

cc = something(
    get(ENV, "CC", nothing),
    Sys.which("cc"),
    Sys.which("gcc"),
    Sys.which("clang"),
)

# Escape paths on windows by using forward slash so we don't get into escaping hell
pathesc(path::String) = replace(path, "\\" => "/")

@static if Sys.isfreebsd()
    make = "gmake"
else
    make = "make"
end

needs_m32() = startswith(capture_output(`$(cc) -dumpmachine`), "x86_64") && Sys.WORD_SIZE == 32


# Build blastrampoline into a temporary directory, and return that
blastrampoline_build_dir = nothing
function get_blastrampoline_dir()
    if blastrampoline_build_dir !== nothing
        return blastrampoline_build_dir
    end

    cflags_add = needs_m32() ? "-m32" : ""
    dir = mktempdir()
    srcdir = joinpath(dirname(@__DIR__), "src")
    run(`$(make) -sC $(pathesc(srcdir)) CFLAGS_add=$(cflags_add) ARCH=$(Sys.ARCH) clean`)
    run(`$(make) -sC $(pathesc(srcdir)) CFLAGS_add=$(cflags_add) ARCH=$(Sys.ARCH) install builddir=$(pathesc(dir))/build prefix=$(pathesc(dir))/output`)
    global blastrampoline_build_dir = joinpath(dir, "output")
    return blastrampoline_build_dir
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
    exported_symbols::Ptr{Cstring}
    num_exported_symbols::UInt32
end
const LBT_BUILDFLAGS_F2C_CAPABLE = 0x02

function lbt_forward(handle, path; clear::Bool = false, verbose::Bool = false)
    ccall(dlsym(handle, :lbt_forward), Int32, (Cstring, Int32, Int32), path, clear ? 1 : 0, verbose ? 1 : 0)
end

function lbt_get_config(handle)
    return unsafe_load(ccall(dlsym(handle, :lbt_get_config), Ptr{lbt_config_t}, ()))
end

function lbt_get_num_threads(handle)
    return ccall(dlsym(handle, :lbt_get_num_threads), Int32, ())
end

function lbt_set_num_threads(handle, nthreads)
    return ccall(dlsym(handle, :lbt_set_num_threads), Cvoid, (Int32,), nthreads)
end

function lbt_get_forward(handle, symbol_name, interface, f2c = LBT_F2C_PLAIN)
    return ccall(dlsym(handle, :lbt_get_forward), Ptr{Cvoid}, (Cstring, Int32, Int32), symbol_name, interface, f2c)
end

function lbt_set_forward(handle, symbol_name, addr, interface, f2c = LBT_F2C_PLAIN; verbose::Bool = false)
    return ccall(dlsym(handle, :lbt_set_forward), Int32, (Cstring, Ptr{Cvoid}, Int32, Int32, Int32), symbol_name, addr, interface, f2c, verbose ? 1 : 0)
end

function lbt_set_default_func(handle, addr)
    return ccall(dlsym(handle, :lbt_set_default_func), Cvoid, (Ptr{Cvoid},), addr)
end

function lbt_get_default_func(handle)
    return ccall(dlsym(handle, :lbt_get_default_func), Ptr{Cvoid}, ())
end
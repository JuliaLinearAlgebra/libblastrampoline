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

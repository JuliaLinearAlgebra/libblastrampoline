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

@static if Sys.iswindows() && endswith(strip(capture_output(`$(cc) -dumpmachine`)), "-cygwin")
    cygpath(path::String) = strip(capture_output(`cygpath $(path)`))
else
    cygpath(path::String) = path
end

# Build blastrampoline into a temporary directory, and return that
blastrampoline_build_dir = nothing
function get_blastrampoline_dir()
    if blastrampoline_build_dir !== nothing
        return blastrampoline_build_dir
    end

    dir = mktempdir()
    srcdir = joinpath(dirname(@__DIR__), "src")
    run(`make -sC $(cygpath(srcdir)) clean`)
    run(`make -sC $(cygpath(srcdir)) install builddir=$(cygpath(dir))/build prefix=$(cygpath(dir))/output`)
    global blastrampoline_build_dir = joinpath(dir, "output")
    return blastrampoline_build_dir
end
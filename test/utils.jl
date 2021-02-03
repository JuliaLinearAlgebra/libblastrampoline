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

# Build blastrampoline into a temporary directory, and return that
blastrampoline_build_dir = nothing
function get_blastrampoline_dir()
    if blastrampoline_build_dir !== nothing
        return blastrampoline_build_dir
    end

    dir = mktempdir()
    srcdir = joinpath(dirname(@__DIR__), "src")
    run(`make -sC $(srcdir) clean`)
    run(`make -sC $(srcdir) install builddir=$(dir)/build prefix=$(dir)/output`)
    global blastrampoline_build_dir = joinpath(dir, "output")
    return blastrampoline_build_dir
end
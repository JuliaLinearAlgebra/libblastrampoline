using OpenBLAS_jll, CompilerSupportLibraries_jll, Pkg, Artifacts

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

function gen_libpath_dict(dirs::Vector{String})
    push!(dirs, get(ENV, LIBPATH_env, expanduser(LIBPATH_default)))
    return Dict(LIBPATH_env => join(dirs, pathsep))
end

# Get libopenblas (including headers) on all Julia versions
function get_stdlib_jll_artifacts_toml(stdlib::Module)
    stdlib_artifacts_toml_path = joinpath(dirname(dirname(Base.pathof(stdlib))), "StdlibArtifacts.toml")
    if isfile(stdlib_artifacts_toml_path)
        return stdlib_artifacts_toml_path
    end
    artifacts_toml_path = joinpath(dirname(dirname(Base.pathof(stdlib))), "Artifacts.toml")
    if isfile(artifacts_toml_path)
        return artifacts_toml_path
    end
    error("Unable to find (Stdlib)Artifacts.toml file for $(stdlib)!")
end

# Returns the artifact directory for OpenBLAS_jll on v1.6+
function get_stdlib_jll_dir(stdlib::Module)
    artifacts_toml = get_stdlib_jll_artifacts_toml(stdlib)
    artifacts = select_downloadable_artifacts(artifacts_toml)
    @assert length(artifacts) == 1
    name = first(keys(artifacts))
    Pkg.Artifacts.ensure_artifact_installed(name, artifacts[name], artifacts_toml)
    return artifact_path(Base.SHA1(artifacts[name]["git-tree-sha1"]))
end

# Build blastrampoline into a temporary directory, and return that
function get_blastrampoline_dir()
    dir = mktempdir()
    build_dir = joinpath(dir, "src")
    cp(joinpath(dirname(@__DIR__), "src"), build_dir)
    cd(build_dir) do
        run(`make clean`)
        run(`make install prefix=$(dir)/output`)
    end
    return joinpath(dir, "output")
end

# Compile `dgemm_test.c` against the given libblas
function dgemm_test(libblas_name, blas_dir)
    dir = mktempdir()
    cp(joinpath(@__DIR__, "dgemm_test"), dir; force=true)

    # We need to configure this C build a bit
    cflags = String[
        "-I$(blas_dir)/include",
    ]
    if Sys.WORD_SIZE == 64
        push!(cflags, "-DILP64")
    end
    
    csl_dir = get_stdlib_jll_dir(CompilerSupportLibraries_jll)
    ldflags = String[
        # Teach it to find that libblas at build and runtime
        "-L$(blas_dir)/$(binlib)",
        "-Wl,-rpath,$(blas_dir)/$(binlib)",
        "-l$(libblas_name)",

        # Teach it to find `libgfortran` at build time (sadly, we can't
        # teach it to find `libgfortran` at run time here, because rpath
        # is not transitive)
        "-Wl,-rpath-link,$(csl_dir)/$(binlib)"
    ]

    cd(dir) do
        @info("Compiling against $(libblas_name) in $(dir)")
        run(`make CFLAGS="$(join(cflags, " "))" LDFLAGS="$(join(ldflags, " "))"`)
        @info("Running in environment that feeds CSL libdir:")
        run(addenv(`./dgemm_test`, gen_libpath_dict([joinpath(csl_dir, binlib)])))
    end
end

# Build version that links against vanilla OpenBLAS
openblas_name = "openblas"
if Sys.WORD_SIZE == 64
    openblas_name = string(openblas_name, "64_")
end
openblas_dir = get_stdlib_jll_dir(OpenBLAS_jll)
dgemm_test(openblas_name, openblas_dir)

# Next, build a version that links against `libblastrampoline`, and tell
# the trampoline to forwards calls to `libopenblas`
withenv("LIBBLAS_NAME" => joinpath(openblas_dir, binlib, string("lib", openblas_name, ".", shlib_ext))) do
    dgemm_test("blastrampoline", get_blastrampoline_dir())
end
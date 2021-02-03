using OpenBLAS_jll, OpenBLAS32_jll, MKL_jll, CompilerSupportLibraries_jll
using Pkg, Artifacts, Base.BinaryPlatforms, Libdl

include("utils.jl")

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

function capture_output(cmd::Cmd)
    out_pipe = Pipe()
    ld_env = filter(e -> startswith(e, "LBT_") || startswith(e, "LD_") || startswith(e, "DYLD_"), cmd.env)
    @info("Running $(basename(cmd.exec[1]))", ld_env)
    p = run(pipeline(ignorestatus(cmd), stdout=out_pipe, stderr=out_pipe), wait=false)
    close(out_pipe.in)
    output = @async read(out_pipe, String)
    wait(p)
    return fetch(output)
end

# Compile `dgemm_test.c` and `sgesv_test.c` against the given BLAS/LAPACK
function run_test((test_name, test_expected_outputs), libblas_name, libdirs, interface, backing_libs)
    p = HostPlatform()
    target = triplet(Platform(arch(p), os(p); libc=libc(p)))

    # We need to configure this C build a bit
    cflags = String[
        "-g",
    ]
    if interface == :ILP64
        push!(cflags, "-DILP64")
    end
    
    ldflags = String[
        # Teach it to find that libblas and its dependencies at build time
        ("-L$(libdir)" for libdir in libdirs)...,
        "-l$(libblas_name)",
    ]

    if !Sys.iswindows()
        # Teach it to find that libblas and its dependencies at run time
        append!(ldflags, ("-Wl,-rpath,$(libdir)" for libdir in libdirs))
    end

    mktempdir() do dir
        @info("Compiling `$(test_name)` against $(libblas_name) in $(dir)")
        srcdir = joinpath(@__DIR__, test_name)
        p = run(ignorestatus(`make -sC $(srcdir) prefix=$(dir) CFLAGS="$(join(cflags, " "))" LDFLAGS="$(join(ldflags, " "))"`))
        if !success(p)
            @error("compilation failed", srcdir, prefix=dir, cflags=join(cflags, " "), ldflags=join(ldflags, " "))
        end
        env = Dict(
            LIBPATH_env => join(dirs, pathsep),
            "LBT_DEFAULT_LIBS" => backing_libs,
        )
        cmd = `$(dir)/$(test_name)`
        output = capture_output(addenv(cmd, env))

        # Test to make sure the test ran properly
        if !all(occursin(expected, output) for expected in test_expected_outputs)
            # Uh-oh, we didn't get what we expected.  Time to debug!
            @error("Test failed, got output:")
            println(output)

            # If we're not on CI, launch `gdb`
            if isempty(get(ENV, "CI", ""))
                @warn("Launching `gdb`")
                cmd = `gdb $(cmd)`
                env["LBT_VERBOSE"] = "1"
                run(addenv(cmd, env))
                exit(1)
            end
        end
    end
end

# our tests
dgemm = ("dgemm_test", ("||C||^2 is:  24.3384",))
sgesv = ("sgesv_test", ("||b||^2 is:   3.0000",))

# Build version that links against vanilla OpenBLAS64
openblas_name = "openblas"
interface = :LP64
if Sys.WORD_SIZE == 64
    openblas_name = string(openblas_name, "64_")
    interface = :ILP64
end
run_test(dgemm, openblas_name, OpenBLAS_jll.LIBPATH_list, interface, "")
run_test(sgesv, openblas_name, OpenBLAS_jll.LIBPATH_list, interface, "")

# Build version that links against vanilla OpenBLAS32
run_test(dgemm, "openblas", OpenBLAS32_jll.LIBPATH_list, :LP64, "")
run_test(sgesv, "openblas", OpenBLAS32_jll.LIBPATH_list, :LP64, "")

# Next, build a version that links against `libblastrampoline`, and tell
# the trampoline to forwards calls to `OpenBLAS_jll`
lbt_dir = joinpath(get_blastrampoline_dir(), binlib)
libdirs = vcat(OpenBLAS_jll.LIBPATH_list..., lbt_dir)
run_test(dgemm, "blastrampoline", libdirs, interface, OpenBLAS_jll.libopenblas_path)
run_test(sgesv, "blastrampoline", libdirs, interface, OpenBLAS_jll.libopenblas_path)

# And again, but this time with OpenBLAS32_jll
libdirs = vcat(OpenBLAS32_jll.LIBPATH_list..., lbt_dir)
run_test(dgemm, "blastrampoline", libdirs, :LP64, OpenBLAS32_jll.libopenblas_path)
run_test(sgesv, "blastrampoline", libdirs, :LP64, OpenBLAS32_jll.libopenblas_path)

# Test against MKL_jll using `libmkl_rt`, which is :LP64 by default
# Note that MKL has dependencies itself, so we need to 
libdirs = vcat(MKL_jll.LIBPATH_list..., lbt_dir)
run_test(dgemm, "blastrampoline", libdirs, :LP64, MKL_jll.libmkl_rt_path)
run_test(sgesv, "blastrampoline", libdirs, :LP64, MKL_jll.libmkl_rt_path)

# Do we have a `blas64.so` somewhere?  If so, test with that for fun
blas64 = dlopen("libblas64", throw_error=false)
if blas64 !== nothing
    run_test(dgemm, "blastrampoline", [lbt_dir], :ILP64, dlpath(blas64))
    # Can't run `sgesv` here as we don't have LAPACK symbols in `libblas64.so`

    # Check if we have a `liblapack` and if we do, run with BOTH
    lapack = dlopen("liblapack64", throw_error=false)
    if lapack !== nothing
        run_test(dgemm, "blastrampoline", [lbt_dir], :ILP64, "$(dlpath(blas64)):$(dlpath(lapack))")
        run_test(sgesv, "blastrampoline", [lbt_dir], :ILP64, "$(dlpath(blas64)):$(dlpath(lapack))")
    end
end

# Finally the super-crazy test: build a binary that links against BOTH sets of symbols!
inconsolable = ("inconsolable_test", ("||C||^2 is:  24.3384", "||b||^2 is:   3.0000"))
libdirs = vcat(OpenBLAS32_jll.LIBPATH_list..., OpenBLAS_jll.LIBPATH_list..., lbt_dir)
run_test(inconsolable, "blastrampoline", libdirs, :wild_sobbing, "$(OpenBLAS32_jll.libopenblas_path):$(OpenBLAS_jll.libopenblas_path)")

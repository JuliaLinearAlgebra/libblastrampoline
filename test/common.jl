# Shared test harness for all backends.  Each backend's `runtests.jl` includes this
# file (which in turn includes `utils.jl`), then `using`s only the JLLs relevant to
# that backend.  This keeps every backend in its own process with its own Project.toml
# so that only a single set of BLAS/LAPACK libraries is ever loaded at once.
using Pkg, Artifacts, Base.BinaryPlatforms, Libdl, Test

include("utils.jl")

# Compile `dgemm_test.c` and `sgesv_test.c` against the given BLAS/LAPACK
function run_test((test_name, test_expected_outputs, expect_success), libblas_name, libdirs, interface, backing_libs; extra_env = Dict())
    # We need to configure this C build a bit
    cflags = String[
        "-g",
    ]
    if interface == :ILP64
        push!(cflags, "-DILP64")
    end

   # If our GCC is 64-bit but Julia is 32-bit, pass -m32
   if needs_m32()
       push!(cflags, "-m32")
   end

    ldflags = String[
        # Teach it to find that libblas and its dependencies at build time
        ("\"-L$(pathesc(libdir))\"" for libdir in libdirs)...,
        "-l$(libblas_name)",
    ]

    if !Sys.iswindows()
        # Teach it to find that libblas and its dependencies at run time
        append!(ldflags, ("-Wl,-rpath,$(pathesc(libdir))" for libdir in libdirs))
    end

    if Sys.islinux()
        # Linux needs this for transitive dependencies
        append!(ldflags, ("-Wl,-rpath-link,$(pathesc(libdir))" for libdir in libdirs))
    end

    mktempdir() do dir
        @info("Compiling `$(test_name)` against $(libblas_name) ($(backing_libs)) in $(dir)")
        srcdir = joinpath(@__DIR__, test_name)
        make_cmd = `$(make) -sC $(pathesc(srcdir)) prefix=$(pathesc(dir)) CFLAGS="$(join(cflags, " "))" LDFLAGS="$(join(ldflags, " "))"`
        p = run(ignorestatus(make_cmd))
        if !success(p)
            @error("compilation failed", srcdir, prefix=dir, cflags=join(cflags, " "), ldflags=join(ldflags, " "))
        end
        @test success(p)

        env = Dict(
            # We need to tell it how to find CSL at run-time
            LIBPATH_env => append_libpath(libdirs),
            "LBT_DEFAULT_LIBS" => backing_libs,
            "LBT_STRICT" => 1,
            "LBT_VERBOSE" => 1,
            pairs(extra_env)...,
        )
        cmd = `$(dir)/$(test_name)`
        p, output = capture_output(addenv(cmd, env))

        expected_return_value = !xor(success(p), expect_success)
        if !expected_return_value
            @error("Test failed", env, p.exitcode, p.termsignal, expect_success)
            println(output)
        end
        @test expected_return_value

        # Expect to see the path to `libblastrampoline` within the output,
        # since we have `LBT_VERBOSE=1` and at startup, it announces its own path:
        if startswith(libblas_name, "blastrampoline") && expect_success
            lbt_libdir = first(libdirs)
            @test occursin(lbt_libdir, output)
        end

        # Test to make sure the test ran properly
        has_expected_output = all(occursin(expected, output) for expected in test_expected_outputs)
        if !has_expected_output
            # Uh-oh, we didn't get what we expected.  Time to debug!
            @error("Test failed, got output:")
            println(output)

            # If we're not on CI, launch `gdb`
            if isempty(get(ENV, "CI", ""))
                debugger = Sys.isbsd() ? "lldb" : "gdb"
                @warn("Launching $debugger")
                cmd = `$(debugger) $(cmd)`
                run(addenv(cmd, env))
            end
        end
        @test has_expected_output
    end
end

# our tests, written in C, defined in subdirectories in `test/`
dgemm =         ("dgemm_test", ("||C||^2 is:  24.3384",),                          true)
dgemmt =        ("dgemmt_test", ("||C||^2 is:  23.2952",),                         true)
dpstrf =        ("dpstrf_test", ("diag(A):   2.2601   1.8067   1.6970   0.4121",), true)
sgesv =         ("sgesv_test", ("||b||^2 is:   3.0000",),                          true)
sgesv_failure = ("sgesv_test", ("Error: no BLAS/LAPACK library loaded!",),         false)
sdot  =         ("sdot_test",  ("C is:   1.9900",),                                true)
cdotc =         ("cdotc_test", (
                     "C (cblas) is:   (  1.4700,   3.8300)",
                     "C (fortran) is: (  1.4700,   3.8300)",
                ),      true)

# Helper function to run all the tests with the given arguments
# Does not include `dgemmt` because that's MKL-only
function run_all_tests(args...; tests = [dgemm, dpstrf, sgesv, sdot, cdotc], kwargs...)
    for test in tests
        run_test(test, args...; kwargs...)
    end
end

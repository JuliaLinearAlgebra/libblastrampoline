using OpenBLAS_jll, OpenBLAS32_jll, MKL_jll, CompilerSupportLibraries_jll
using Pkg, Artifacts, Base.BinaryPlatforms, Libdl, Test

include("utils.jl")

# Compile `dgemm_test.c` and `sgesv_test.c` against the given BLAS/LAPACK
function run_test((test_name, test_expected_outputs, test_success), libblas_name, libdirs, interface, backing_libs)
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
        ("-L$(pathesc(libdir))" for libdir in libdirs)...,
        "-l$(libblas_name)",
    ]

    if !Sys.iswindows()
        # Teach it to find that libblas and its dependencies at run time
        append!(ldflags, ("-Wl,-rpath,$(pathesc(libdir))" for libdir in libdirs))
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
        )
        cmd = `$(dir)/$(test_name)`
        p, output = capture_output(addenv(cmd, env))

        expected_return_value = success(p) ^ test_success
        if !expected_return_value
            @error("Test failed", env)
            println(output)
        end
        @test expected_return_value

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
dgemm =         ("dgemm_test", ("||C||^2 is:  24.3384",),                  true)
sgesv =         ("sgesv_test", ("||b||^2 is:   3.0000",),                  true)
sgesv_failure = ("sgesv_test", ("Error: no BLAS/LAPACK library loaded!",), false)
sdot  =         ("sdot_test",  ("C is:   1.9900",),                        true)
zdotc =         ("zdotc_test", (
                     "C (cblas) is:   (  1.4700,   3.8300)",
                     "C (fortran) is: (  1.4700,   3.8300)",
                ),      true)

# Helper function to run all the tests with the given arguments
function run_all_tests(args...; tests = [dgemm, sgesv, sdot, zdotc])
    for test in tests
        run_test(test, args...)
    end
end

# Build version that links against vanilla OpenBLAS
openblas_interface = :LP64
if Sys.WORD_SIZE == 64 && Sys.ARCH != :aarch64
    openblas_interface = :ILP64
end
openblas_jll_libname = splitext(basename(OpenBLAS_jll.libopenblas_path)[4:end])[1]
@testset "Vanilla OpenBLAS_jll ($(openblas_interface))" begin
    run_all_tests(openblas_jll_libname, OpenBLAS_jll.LIBPATH_list, openblas_interface, "")
end

# Build version that links against vanilla OpenBLAS32
@testset "Vanilla OpenBLAS32_jll (LP64)" begin
    # Reverse OpenBLAS32_jll's LIBPATH_list so that we get the right openblas.so
    run_all_tests("openblas", reverse(OpenBLAS32_jll.LIBPATH_list), :LP64, "")
end

# Next, build a version that links against `libblastrampoline`, and tell
# the trampoline to forwards calls to `OpenBLAS_jll`
lbt_link_name, lbt_dir = build_libblastrampoline()
lbt_dir = joinpath(lbt_dir, binlib)

@testset "LBT -> OpenBLAS_jll ($(openblas_interface))" begin
    libdirs = unique(vcat(lbt_dir, OpenBLAS_jll.LIBPATH_list..., CompilerSupportLibraries_jll.LIBPATH_list...))
    run_all_tests("blastrampoline", libdirs, openblas_interface, OpenBLAS_jll.libopenblas_path)
end

# And again, but this time with OpenBLAS32_jll
@testset "LBT -> OpenBLAS32_jll (LP64)" begin
    libdirs = unique(vcat(lbt_dir, OpenBLAS32_jll.LIBPATH_list..., CompilerSupportLibraries_jll.LIBPATH_list...))
    run_all_tests("blastrampoline", libdirs, :LP64, OpenBLAS32_jll.libopenblas_path)
end

# Test against MKL_jll using `libmkl_rt`, which is :LP64 by default
if MKL_jll.is_available()
    @testset "LBT -> MKL_jll (LP64)" begin
        libdirs = unique(vcat(lbt_dir, MKL_jll.LIBPATH_list..., CompilerSupportLibraries_jll.LIBPATH_list...))
        run_all_tests("blastrampoline", libdirs, :LP64, MKL_jll.libmkl_rt_path)
    end

    # Test that we can set MKL's interface via an environment variable to select ILP64, and LBT detects it properly
    if Sys.WORD_SIZE == 64
        @testset "LBT -> MKL_jll (ILP64, via env)" begin
            withenv("MKL_INTERFACE_LAYER" => "ILP64") do
                libdirs = unique(vcat(lbt_dir, MKL_jll.LIBPATH_list..., CompilerSupportLibraries_jll.LIBPATH_list...))
                run_all_tests("blastrampoline", libdirs, :ILP64, MKL_jll.libmkl_rt_path)
            end
        end

        @testset "LBT -> MKL_jll (ILP64, via suffix)" begin
            withenv("LBT_SUFFIX_HINT" => "64", "LBT_VERBOSE" => "true") do
                libdirs = unique(vcat(lbt_dir, MKL_jll.LIBPATH_list..., CompilerSupportLibraries_jll.LIBPATH_list...))
                run_all_tests("blastrampoline", libdirs, :ILP64, MKL_jll.libmkl_rt_path)
            end
        end
    end
end

# Do we have Accelerate available?
veclib_blas_path = "/System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/libBLAS.dylib"
if isfile(veclib_blas_path)
    # Test that we can run BLAS-only tests without LAPACK loaded (`sgesv` test requires LAPACK symbols)
    @testset "LBT -> vecLib/libBLAS" begin
        run_all_tests("blastrampoline", [lbt_dir], :LP64, veclib_blas_path; tests=[dgemm, sdot, zdotc])
    end

    # With LAPACK as well, run all tests
    veclib_lapack_path = "/System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/libLAPACK.dylib"
    @testset "LBT -> vecLib/libLAPACK" begin
        run_all_tests("blastrampoline", [lbt_dir], :LP64, string(veclib_blas_path, ";", veclib_lapack_path))
    end
end


# Do we have a `blas64.so` somewhere?  If so, test with that for fun
blas64 = dlopen("libblas64", throw_error=false)
if blas64 !== nothing
    # Test that we can run BLAS-only tests without LAPACK loaded (`sgesv` test requires LAPACK symbols, blas64 doesn't have CBLAS)
    @testset "LBT -> libblas64 (ILP64, BLAS)" begin
        run_all_tests("blastrampoline", [lbt_dir], :ILP64, dlpath(blas64); tests=[dgemm, sdot])
    end

    # Check if we have a `liblapack` and if we do, run again, this time including `sgesv`
    lapack = dlopen("liblapack64", throw_error=false)
    if lapack !== nothing
        @testset "LBT -> libblas64 + liblapack64 (ILP64, BLAS+LAPACK)" begin
            run_all_tests("blastrampoline", [lbt_dir], :ILP64, "$(dlpath(blas64));$(dlpath(lapack))"; tests=[dgemm, sdot, sgesv])
        end
    end
end

# Finally the super-crazy test: build a binary that links against BOTH sets of symbols!
if openblas_interface == :ILP64
    inconsolable = ("inconsolable_test", ("||C||^2 is:  24.3384", "||b||^2 is:   3.0000"), true)
    @testset "LBT -> OpenBLAS 32 + 64 (LP64 + ILP64)" begin
        libdirs = unique(vcat(OpenBLAS32_jll.LIBPATH_list..., OpenBLAS_jll.LIBPATH_list..., CompilerSupportLibraries_jll.LIBPATH_list..., lbt_dir))
        run_test(inconsolable, lbt_link_name, libdirs, :wild_sobbing, "$(OpenBLAS32_jll.libopenblas_path);$(OpenBLAS_jll.libopenblas_path)")
    end
end

# Run our "direct" tests within Julia
include("direct.jl")

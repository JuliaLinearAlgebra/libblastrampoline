include(joinpath(@__DIR__, "..", "..", "common.jl"))

# All of the Accelerate tests are macOS-only, so this whole backend is a no-op elsewhere.
if !Sys.isapple()
    @info("Accelerate is only available on macOS; skipping Accelerate tests")
else

lbt_link_name, lbt_dir = build_libblastrampoline()
lbt_dir = joinpath(lbt_dir, binlib)

# Do we have Accelerate available?  Note that we can't use `isfile()` here, since Apple
# has Helpfully (TM) sequestered the dynamic libraries away into a database somewhere that
# just gets fed into `dlopen()` when you ask for that magic path.
veclib_blas_path = "/System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/libBLAS.dylib"
if dlopen_e(veclib_blas_path) != C_NULL
    # Test that we can run BLAS-only tests without LAPACK loaded (`sgesv` test requires LAPACK symbols)
    @testset "LBT -> vecLib/libBLAS" begin
        run_all_tests(blastrampoline_link_name(), [lbt_dir], :LP64, veclib_blas_path; tests=[dgemm, sdot, cdotc])
    end

    # With LAPACK as well, run all tests except `dgemmt`
    veclib_lapack_path = "/System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/libLAPACK.dylib"
    @testset "LBT -> vecLib/libLAPACK" begin
        run_all_tests(blastrampoline_link_name(), [lbt_dir], :LP64, string(veclib_blas_path, ";", veclib_lapack_path))
    end

    veclib_lapack_handle = dlopen(veclib_lapack_path)
    if dlsym_e(veclib_lapack_handle, "dpotrf\$NEWLAPACK\$ILP64") != C_NULL
        @testset "LBT -> vecLib/libBLAS (ILP64)" begin
            veclib_blas_path_ilp64 = "$(veclib_blas_path)!\x1a\$NEWLAPACK\$ILP64"
            run_all_tests(blastrampoline_link_name(), [lbt_dir], :ILP64, veclib_blas_path_ilp64; tests=[dgemm, sdot, cdotc])
        end

        @testset "LBT -> vecLib/libLAPACK (ILP64)" begin
            veclib_lapack_path_ilp64 = "$(veclib_lapack_path)!\x1a\$NEWLAPACK\$ILP64"
            @warn("dpstrf test broken on new LAPACK in Accelerate")
            dpstrf_broken = (dpstrf[1], "diag(A):   2.2601   1.7140   0.6206   1.1878", true)
            run_all_tests(blastrampoline_link_name(), [lbt_dir], :ILP64, veclib_lapack_path_ilp64; tests=[dgemm, dpstrf_broken, sgesv, sdot, cdotc])
        end
    end
end

# In-process Accelerate tests, driven through the LBT C API directly.
# Taken from AppleAccelerate.jl to avoid a dependency on it
const libacc = "/System/Library/Frameworks/Accelerate.framework/Accelerate"
const libacc_info_plist = "/System/Library/Frameworks/Accelerate.framework/Versions/Current/Resources/Info.plist"

function get_macos_version(normalize=true)
    plist_lines = split(String(read("/System/Library/CoreServices/SystemVersion.plist")), "\n")
    vers_idx = findfirst(l -> occursin("ProductVersion", l), plist_lines)
    if vers_idx === nothing
        return nothing
    end

    m = match(r">([\d\.]+)<", plist_lines[vers_idx+1])
    if m === nothing
        return nothing
    end

    ver = VersionNumber(only(m.captures))
    if normalize && ver.major == 16
        return VersionNumber(26, ver.minor, ver.patch)
    end
    return ver
end

# Load libblastrampoline and the Accelerate library
lbt_handle = open_lbt_handle()
libacc_handle = dlopen(libacc)

@testset "Accelerate ILP64 loading" begin
    # ILP64 requires macOS 13.3+
    if get_macos_version() >= v"13.3"
        # Load the ILP64 interface
        lbt_forward(lbt_handle, libacc; clear=true, suffix_hint="\x1a\$NEWLAPACK\$ILP64")

        # Test that we have only one library loaded
        config = lbt_get_config(lbt_handle)
        libs = unpack_loaded_libraries(config)
        @test length(libs) == 1

        # Test that it's Accelerate and it's correctly identified
        @test libs[1].libname == libacc
        @test libs[1].interface == LBT_INTERFACE_ILP64

        # Test that `dgemm` forwards to `dgemm_` within the Accelerate library
        acc_dgemm = dlsym(libacc_handle, "dgemm\$NEWLAPACK\$ILP64")
        @test lbt_get_forward(lbt_handle, "dgemm_", LBT_INTERFACE_ILP64) == acc_dgemm
    end
end

@testset "Accelerate LP64 loading" begin
    # New LAPACK interface requires macOS 13.3+
    if get_macos_version() >= v"13.3"
        # Load the LP64 interface
        lbt_forward(lbt_handle, libacc; clear=true, suffix_hint="\x1a\$NEWLAPACK")

        # Test that we have only one library loaded
        config = lbt_get_config(lbt_handle)
        libs = unpack_loaded_libraries(config)
        @test length(libs) == 1

        # Test that it's Accelerate and it's correctly identified
        @test libs[1].libname == libacc
        @test libs[1].interface == LBT_INTERFACE_LP64

        # Test that `dgemm` forwards to `dgemm_` within the Accelerate library
        acc_dgemm = dlsym(libacc_handle, "dgemm\$NEWLAPACK")
        @test lbt_get_forward(lbt_handle, "dgemm_", LBT_INTERFACE_LP64) == acc_dgemm
    end
end

@testset "Accelerate threading" begin
    # This threading API will only work on v15 and above
    if get_macos_version() >= v"15"
        lbt_forward(lbt_handle, libacc; clear=true)

        # Set to single-threaded
        lbt_set_num_threads(lbt_handle, 1)
        @test lbt_get_num_threads(lbt_handle) == 1

        # Set to multi-threaded
        # Accelerate doesn't actually let us say how many threads, so we must test for greater than
        lbt_set_num_threads(lbt_handle, 2)
        @test lbt_get_num_threads(lbt_handle) > 1

        # Set back to single-threaded
        lbt_set_num_threads(lbt_handle, 1)
        @test lbt_get_num_threads(lbt_handle) == 1
    end
end

end # if Sys.isapple()

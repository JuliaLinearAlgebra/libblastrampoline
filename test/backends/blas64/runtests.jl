include(joinpath(@__DIR__, "..", "..", "common.jl"))

# Test against a system-provided `libblas64`/`liblapack64` (ILP64), e.g. the Debian
# `libblas64-dev` / `liblapack64-dev` packages.  This is gated on those libraries
# actually being installed, so the backend is a no-op when they are missing.
lbt_link_name, lbt_dir = build_libblastrampoline()
lbt_dir = joinpath(lbt_dir, binlib)

blas64 = dlopen("libblas64", throw_error=false)
if blas64 !== nothing
    # Test that we can run BLAS-only tests without LAPACK loaded (`sgesv` test requires LAPACK symbols, blas64 doesn't have CBLAS)
    @testset "LBT -> libblas64 (ILP64, BLAS)" begin
        run_all_tests(blastrampoline_link_name(), [lbt_dir], :ILP64, dlpath(blas64); tests=[dgemm, sdot])
    end

    # Check if we have a `liblapack` and if we do, run again, this time including `sgesv`
    lapack = dlopen("liblapack64", throw_error=false)
    if lapack !== nothing
        @testset "LBT -> libblas64 + liblapack64 (ILP64, BLAS+LAPACK)" begin
            run_all_tests(blastrampoline_link_name(), [lbt_dir], :ILP64, "$(dlpath(blas64));$(dlpath(lapack))"; tests=[dgemm, sdot, sgesv])
        end
    end
else
    @info("No system `libblas64` found; skipping libblas64 tests")
end

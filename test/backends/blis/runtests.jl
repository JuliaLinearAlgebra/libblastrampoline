include(joinpath(@__DIR__, "..", "..", "common.jl"))
using blis_jll, CompilerSupportLibraries_jll

# BLIS is an ILP64, BLAS-only library (it ships `dgemm_64_` etc., but no CBLAS and no
# LAPACK), so we only exercise the BLAS routines here.
#
# Pairing BLIS with a separate LAPACK to test `dpstrf`/`sgesv` is blocked on the same
# issue as the ILP64 reference backend: the ILP64 `LAPACK_jll` artifact exports both the
# plain and `_64_`-suffixed symbols, so LBT autodetects it as LP64 and never forwards the
# ILP64 LAPACK symbols.  Re-enable a `blis + LAPACK` testset once that is fixed upstream.
if blis_jll.is_available() && Sys.WORD_SIZE == 64
    lbt_link_name, lbt_dir = build_libblastrampoline()
    lbt_dir = joinpath(lbt_dir, binlib)

    @testset "LBT -> blis_jll (ILP64, BLAS)" begin
        libdirs = unique(vcat(lbt_dir, blis_jll.LIBPATH_list..., CompilerSupportLibraries_jll.LIBPATH_list...))
        run_all_tests(blastrampoline_link_name(), libdirs, :ILP64, blis_jll.blis_path; tests = [dgemm, sdot])
    end
else
    @info("blis_jll is not available on this platform; skipping BLIS tests")
end

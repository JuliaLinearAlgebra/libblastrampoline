include(joinpath(@__DIR__, "..", "..", "common.jl"))
using ReferenceBLAS32_jll, LAPACK32_jll, CompilerSupportLibraries_jll
# NB: `ReferenceBLAS_jll` / `LAPACK_jll` (ILP64) are intentionally not loaded here; see
# the commented-out ILP64 testset below.

# Reference BLAS/LAPACK has no CBLAS layer, so we skip the `cdotc` test everywhere.

lbt_link_name, lbt_dir = build_libblastrampoline()
lbt_dir = joinpath(lbt_dir, binlib)

# --- LP64: ReferenceBLAS32_jll / LAPACK32_jll ---
@testset "Vanilla ReferenceBLAS32_jll (LP64)" begin
    run_all_tests("blas32", reverse(ReferenceBLAS32_jll.LIBPATH_list), :LP64, "", tests = [dgemm, sdot])
end

@testset "LBT -> ReferenceBLAS32_jll / LAPACK32_jll (LP64)" begin
    libdirs = unique(vcat(lbt_dir, ReferenceBLAS32_jll.LIBPATH_list..., LAPACK32_jll.LIBPATH_list..., CompilerSupportLibraries_jll.LIBPATH_list...))
    run_all_tests(blastrampoline_link_name(), libdirs, :LP64, string(ReferenceBLAS32_jll.libblas32_path, ";", LAPACK32_jll.liblapack32_path); tests = [dgemm, dpstrf, sgesv, sdot])
end

# --- ILP64: ReferenceBLAS_jll / LAPACK_jll ---
# Disabled for now: the ILP64 `LAPACK_jll`/`ReferenceBLAS_jll` artifacts export *both*
# the plain (`ilaver_`) and the `_64_`-suffixed (`ilaver_64_`) symbols, so LBT's
# autodetection identifies them as LP64 and never forwards the ILP64 LAPACK symbols
# (the test then fails with `no BLAS/LAPACK library loaded for ilaver_()`).  This needs
# the ILP64 reference libraries to be rebuilt in Yggdrasil with a proper `64_` symbol
# suffix (and `libblas64`/`liblapack64` sonames); tracked in
# https://github.com/JuliaLinearAlgebra/libblastrampoline/pull/128
#
# if Sys.WORD_SIZE == 64
#     @testset "LBT -> ReferenceBLAS_jll / LAPACK_jll (ILP64)" begin
#         libdirs = unique(vcat(lbt_dir, ReferenceBLAS_jll.LIBPATH_list..., LAPACK_jll.LIBPATH_list..., CompilerSupportLibraries_jll.LIBPATH_list...))
#         run_all_tests(blastrampoline_link_name(), libdirs, :ILP64, string(ReferenceBLAS_jll.libblas_path, ";", LAPACK_jll.liblapack_path); tests = [dgemm, dpstrf, sgesv, sdot])
#     end
# end

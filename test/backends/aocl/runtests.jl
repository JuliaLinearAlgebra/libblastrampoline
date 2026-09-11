include(joinpath(@__DIR__, "..", "..", "common.jl"))
using AOCL_jll, CompilerSupportLibraries_jll

# AMD AOCL (AOCL-BLAS + AOCL-LAPACK, used in Julia through AOCL.jl) ships two
# libraries containing both BLAS/CBLAS and LAPACK: `libaocl` (LP64) and `libaocl64`
# (ILP64).  Both use plain, unsuffixed symbol names (`dgemm_`, not `dgemm_64_`), so the
# ILP64 build is identified by LBT's runtime `isamax` interface probe rather than by its
# symbol suffix.
if AOCL_jll.is_available()
    lbt_link_name, lbt_dir = build_libblastrampoline()
    lbt_dir = joinpath(lbt_dir, binlib)
    libdirs = unique(vcat(lbt_dir, AOCL_jll.LIBPATH_list..., CompilerSupportLibraries_jll.LIBPATH_list...))

    @testset "LBT -> AOCL_jll (LP64)" begin
        run_all_tests(blastrampoline_link_name(), libdirs, :LP64, AOCL_jll.libaocl_path; tests = [dgemm, dgemmt, dpstrf, sgesv, sdot, cdotc])
    end

    if Sys.WORD_SIZE == 64
        @testset "LBT -> AOCL_jll (ILP64)" begin
            run_all_tests(blastrampoline_link_name(), libdirs, :ILP64, AOCL_jll.libaocl64_path; tests = [dgemm, dgemmt, dpstrf, sgesv, sdot, cdotc])
        end
    end

    # In-process tests, driven through the LBT C API directly
    lbt_handle = open_lbt_handle()

    @testset "AOCL loading" begin
        for (libpath, interface) in ((AOCL_jll.libaocl_path, LBT_INTERFACE_LP64),
                                     (AOCL_jll.libaocl64_path, LBT_INTERFACE_ILP64))
            # `libaocl64` is ILP64 with plain symbol names, so skip it on 32-bit
            if interface == LBT_INTERFACE_ILP64 && Sys.WORD_SIZE != 64
                continue
            end
            lbt_forward(lbt_handle, libpath; clear=true)

            libs = unpack_loaded_libraries(lbt_get_config(lbt_handle))
            @test length(libs) == 1
            @test libs[1].libname == libpath
            @test libs[1].interface == interface

            # AOCL uses plain, unsuffixed symbol names in both interfaces
            aocl_handle = dlopen(libpath)
            @test lbt_get_forward(lbt_handle, "dgemm_", interface) == dlsym(aocl_handle, "dgemm_")
        end
    end

    # AOCL-BLAS is BLIS-based, so it exposes the `bli_thread_{get,set}_num_threads` interface
    # that LBT already knows about.
    @testset "AOCL threading" begin
        lbt_forward(lbt_handle, AOCL_jll.libaocl_path; clear=true)
        for nthreads in (2, 1)
            lbt_set_num_threads(lbt_handle, nthreads)
            @test lbt_get_num_threads(lbt_handle) == nthreads
        end
    end
else
    @info("AOCL_jll is not available on this platform; skipping AOCL tests")
end

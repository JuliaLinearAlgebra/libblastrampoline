include(joinpath(@__DIR__, "..", "..", "common.jl"))
using MKL_jll, CompilerSupportLibraries_jll

# Test against MKL_jll using `libmkl_rt`, which is :LP64 by default
if MKL_jll.is_available()
    lbt_link_name, lbt_dir = build_libblastrampoline()
    lbt_dir = joinpath(lbt_dir, binlib)

    # On i686, we can't do complex return style autodetection, so we manually set it,
    # knowing that MKL is argument-style.
    extra_env = Dict{String,String}()
    if Sys.ARCH == :i686
        extra_env["LBT_FORCE_RETSTYLE"] = "ARGUMENT"
    end
    @testset "LBT -> MKL_jll (LP64)" begin
        libdirs = unique(vcat(lbt_dir, MKL_jll.LIBPATH_list..., CompilerSupportLibraries_jll.LIBPATH_list...))
        run_all_tests(blastrampoline_link_name(), libdirs, :LP64, MKL_jll.libmkl_rt_path; tests = [dgemm, dgemmt, dpstrf, sgesv, sdot, cdotc], extra_env)
    end

    # Test that we can set MKL's interface via an environment variable to select ILP64, and LBT detects it properly
    if Sys.WORD_SIZE == 64
        @testset "LBT -> MKL_jll (ILP64, via env)" begin
            withenv("MKL_INTERFACE_LAYER" => "ILP64") do
                libdirs = unique(vcat(lbt_dir, MKL_jll.LIBPATH_list..., CompilerSupportLibraries_jll.LIBPATH_list...))
                run_all_tests(blastrampoline_link_name(), libdirs, :ILP64, MKL_jll.libmkl_rt_path; tests = [dgemm, dgemmt, dpstrf, sgesv, sdot, cdotc], extra_env)
            end
        end
    end
else
    @info("MKL_jll is not available on this platform; skipping MKL tests")
end

include(joinpath(@__DIR__, "..", "..", "common.jl"))
using OpenBLAS_jll, OpenBLAS32_jll, CompilerSupportLibraries_jll

# Build version that links against vanilla OpenBLAS
openblas_interface = :LP64
if Sys.WORD_SIZE == 64
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
    run_all_tests(blastrampoline_link_name(), libdirs, openblas_interface, OpenBLAS_jll.libopenblas_path)

    # Test that setting bad `LBT_FORCE_*` values actually breaks things
    # This can be somewhat unpredictable (segfaulting sometimes, returning zero other times)
    # so it's hard to test on CI, so we comment it out for now.
    #=
    withenv("LBT_FORCE_RETSTYLE" => "ARGUMENT") do
        cdotc_fail = ("cdotc_test", cdotc[2], false)
        run_test(cdotc_fail, blastrampoline_link_name(), libdirs, openblas_interface, OpenBLAS_jll.libopenblas_path)
    end
    =#

    withenv("LBT_USE_RTLD_DEEPBIND" => "false") do
        dgemm_deepbindless = (
            "dgemm_test",
            ("||C||^2 is:  24.3384", "avoiding usage of RTLD_DEEPBIND"),
            true,
        )
        run_test(dgemm_deepbindless, blastrampoline_link_name(), libdirs, openblas_interface, OpenBLAS_jll.libopenblas_path)
    end
end

# And again, but this time with OpenBLAS32_jll
@testset "LBT -> OpenBLAS32_jll (LP64)" begin
    libdirs = unique(vcat(lbt_dir, OpenBLAS32_jll.LIBPATH_list..., CompilerSupportLibraries_jll.LIBPATH_list...))
    run_all_tests(blastrampoline_link_name(), libdirs, :LP64, OpenBLAS32_jll.libopenblas_path)

    # Test that setting bad `LBT_FORCE_*` values actually breaks things
    # but only on non-armv7l, because our `isamax` test is doubly broken there, due to a
    # `int64_t` return being passed on the stack, thus being filled with half trash.
    if Sys.ARCH != :arm
        withenv("LBT_FORCE_INTERFACE" => "ILP64") do
            # `max_idx: 2` is incorrect, it's what happens when ILP64 data is given to an LP64 backend
            isamax_fail = ("isamax_test", ["max_idx: 2"], true)
            run_test(isamax_fail, blastrampoline_link_name(), libdirs, :ILP64, OpenBLAS32_jll.libopenblas_path)
        end
    end
end

# Finally the super-crazy test: build a binary that links against BOTH sets of symbols!
if openblas_interface == :ILP64
    inconsolable = ("inconsolable_test", ("||C||^2 is:  24.3384", "||b||^2 is:   3.0000"), true)
    @testset "LBT -> OpenBLAS 32 + 64 (LP64 + ILP64)" begin
        libdirs = unique(vcat(lbt_dir, OpenBLAS32_jll.LIBPATH_list..., OpenBLAS_jll.LIBPATH_list..., CompilerSupportLibraries_jll.LIBPATH_list...))
        run_test(inconsolable, lbt_link_name, libdirs, :wild_sobbing, "$(OpenBLAS32_jll.libopenblas_path);$(OpenBLAS_jll.libopenblas_path)")
    end
end

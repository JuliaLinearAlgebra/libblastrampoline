using Libdl, Test, OpenBLAS_jll
include("utils.jl")

function load_blas_funcs(handle, path; clear::Bool = false, verbose::Bool = false)
    ccall(dlsym(handle, :load_blas_funcs), Cint, (Cstring, Cint, Cint), path, clear ? 1 : 0, verbose ? 1 : 0)
end

@testset "Direct usage" begin
    lbt_prefix = get_blastrampoline_dir()
    lbt_handle = dlopen("$(lbt_prefix)/$(binlib)/libblastrampoline.$(shlib_ext)", RTLD_GLOBAL | RTLD_DEEPBIND)
    @test lbt_handle != C_NULL

    load_blas_funcs(lbt_handle, OpenBLAS_jll.libopenblas_path; verbose=true)
end
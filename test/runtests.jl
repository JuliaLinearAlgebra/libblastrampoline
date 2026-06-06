# Top-level dispatcher.  Each BLAS/LAPACK backend lives in its own directory under
# `backends/<name>/` with its own `Project.toml` (pulling in only the JLLs relevant to
# that backend) and its own `runtests.jl`.  Each backend is run in a *separate* Julia
# process so that only a single set of BLAS/LAPACK libraries is ever loaded at once,
# which is exactly how the `.github/workflows/ci.yml` matrix invokes them.
#
# Usage:
#   julia test/runtests.jl                # run every backend, each in its own process
#   julia test/runtests.jl openblas mkl   # run only the named backends
const ALL_BACKENDS = ["openblas", "mkl", "refblas", "blis", "blas64", "accelerate", "direct"]

selected_backends = isempty(ARGS) ? ALL_BACKENDS : ARGS
for backend in selected_backends
    if !(backend in ALL_BACKENDS)
        error("Unknown BLAS/LAPACK backend \"$(backend)\". Available backends: $(join(ALL_BACKENDS, ", "))")
    end
end

const JULIA = Base.julia_cmd()
failed = String[]
for backend in selected_backends
    dir = joinpath(@__DIR__, "backends", backend)
    @info("==================== Running backend: $(backend) ====================")
    instantiate = `$(JULIA) --project=$(dir) -e "import Pkg; Pkg.instantiate()"`
    testcmd = `$(JULIA) --project=$(dir) $(joinpath(dir, "runtests.jl"))`
    ok = success(run(ignorestatus(instantiate))) && success(run(ignorestatus(testcmd)))
    if !ok
        push!(failed, backend)
    end
end

if !isempty(failed)
    error("The following backends had test failures: $(join(failed, ", "))")
end

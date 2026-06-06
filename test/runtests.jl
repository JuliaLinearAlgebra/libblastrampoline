# Top-level test driver.  Each BLAS/LAPACK backend lives in its own directory under
# `backends/<name>/` with its own `Project.toml` (pulling in only the JLLs relevant to
# that backend) and its own `runtests.jl`.
#
# Usage:
#   julia --project=test/backends/openblas test/runtests.jl openblas
#       Runs a single backend *in this process*, using the BLAS/LAPACK JLLs from the
#       active project.  This is how `.github/workflows/ci.yml` invokes each job.
#
#   julia test/runtests.jl
#       Runs every backend, each in its own process with its own project, so that only
#       a single set of BLAS/LAPACK libraries is ever loaded at once.  Handy locally.
#
#   julia test/runtests.jl openblas mkl
#       Runs the named backends, each in its own process/project (same isolation).
using Test

const ALL_BACKENDS = ["openblas", "mkl", "refblas", "blis", "blas64", "accelerate", "direct"]

selected_backends = isempty(ARGS) ? ALL_BACKENDS : ARGS
for backend in selected_backends
    if !(backend in ALL_BACKENDS)
        error("Unknown BLAS/LAPACK backend \"$(backend)\". Available backends: $(join(ALL_BACKENDS, ", "))")
    end
end

if length(selected_backends) == 1 && !isempty(ARGS)
    # A single backend was explicitly requested: run it in *this* process.  The caller is
    # responsible for activating the matching `test/backends/<backend>/Project.toml` so
    # that only that backend's BLAS/LAPACK JLLs are loaded.
    backend = only(selected_backends)
    @info("Running test backend: $(backend)")
    @testset "$(backend)" begin
        include(joinpath(@__DIR__, "backends", backend, "runtests.jl"))
    end
else
    # Multiple (or all) backends: run each in its own process with its own project, so
    # that only a single set of BLAS/LAPACK libraries is ever loaded at once.
    julia = Base.julia_cmd()
    failed = String[]
    for backend in selected_backends
        dir = joinpath(@__DIR__, "backends", backend)
        @info("==================== Running backend: $(backend) ====================")
        instantiate = `$(julia) --project=$(dir) -e "import Pkg; Pkg.instantiate()"`
        testcmd = `$(julia) --project=$(dir) $(@__FILE__) $(backend)`
        ok = success(run(ignorestatus(instantiate))) && success(run(ignorestatus(testcmd)))
        if !ok
            push!(failed, backend)
        end
    end
    if !isempty(failed)
        error("The following backends had test failures: $(join(failed, ", "))")
    end
end

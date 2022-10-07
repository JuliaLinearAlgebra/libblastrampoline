#!/usr/bin/env bash
set -euo pipefail

julia --project=test -e 'import Pkg; Pkg.instantiate()'
echo '+++ runtests.jl'
julia --project=test test/runtests.jl

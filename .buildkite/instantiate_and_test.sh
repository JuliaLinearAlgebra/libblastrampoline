#!/usr/bin/env bash
#set -euo pipefail

echo "+++ DEBUG"
echo "PATH=$PATH"
ls -la /c/buildkite-agent

julia --project=test -e 'import Pkg; Pkg.instantiate()'
echo '+++ runtests.jl'
julia --project=test test/runtests.jl

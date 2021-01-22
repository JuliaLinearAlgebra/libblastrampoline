# Trampoline definitions

These trampoline definitions are taken from the [Julia repository])(https://github.com/JuliaLang/julia/tree/master/cli); they should always be kept in-sync.
Tes, this means that we have a few oddities, such as the files trying to `#include "../../src/jl_exported_funcs.inc"`, but that's fine, because we'll just use that filename to build our list of exported functions.
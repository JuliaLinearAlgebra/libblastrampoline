# Simple libblastrampoline tests

This is a simple battery of tests to ensure the trampolines are working properly.
We have two simple test programs, `dgemm_test` and `sgesv_test`, which test the FORTRAN interfaces of `dgemm_` and `sgesv_` to ensure proper execution.
We compile these tests first against vanilla OpenBLAS installations (both LP64 and ILP64, if available) and then against `libblastrampoline` with various backing BLAS libraries.
If available, we will test against MKL and `libblas64`.

Running these tests currently requires the latest tip of `master` on Julia, to deal with JLL lib paths properly.

Run via:
```
julia --project run-tests.jl
```
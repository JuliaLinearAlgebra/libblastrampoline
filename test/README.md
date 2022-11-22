# Simple libblastrampoline tests

This is a simple battery of tests to ensure the trampolines are working properly.
We have three simple test programs, `dgemm_test`, `dgemmt_test` and `sgesv_test`, which test the FORTRAN interfaces of `dgemm_`, `dgemmt_` and `sgesv_` to ensure proper execution.
We compile these tests first against vanilla OpenBLAS installations (both LP64 and ILP64, if available) and then against `libblastrampoline` with various backing BLAS libraries.
If available, we will test against `MKL` and `libblas64` as well.
If we are on a 64-bit platform, we will build a test case that uses LP64 and ILP64 routines in the same executable.

Note that these tests require Julia v1.7+, especially on aarch64 where we switched from shipping an LP64 to ILP64 OpenBLAS by default.

Run via:
```
julia --project runtests.jl
```

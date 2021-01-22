# libblastrampoline

Proof-of-concept of using trampolines to provide a BLAS muxing library.

## Basic usage

Build `libblastrampoline.so`, then link your BLAS-using library against it instead of `libblas.so`.
When `libblastrampoline` is loaded, it will inspect the `LIBBLAS_NAME` environment variable and attempt to forward BLAS calls made to it on to that library.
At any time, you may call `set_blas_funcs(libname)` to redirect forwarding to a new BLAS library.

## Future work

* Ergonomics of generating API:
    - Right now we're only redirecting `cblas_dgemm64_()`; we need to come up with a clever way to enumerate all of them.
    - We also need to provide header files for clients to compile against us with.
    - Perhaps we just steal the OpenBLAS or MKL header files, then parse those to generate `src/jl_exported_funcs.inc`?

* Split APIs
    - I believe MKL partitions symbols differently from OpenBLAS; we may need to allow for looking up BLAS symbols in one library, LAPACK symbols in another, et c...

* Name mangling
    - We should allow for runtime-settable (and possibly auto-detected) name-mangling behavior, e.g. we should be able to map `dgemm_64 -> dlsym(libmkl, dgemm)`, there's no reason the names have to be identical.

* ILP64 management
    - We should be able to auto-detect whether a BLAS library is ILP64 or not by following this recipe:
        - If `dgemm_64` exists within it, it's ILP64
        - If `dgemm` does not exist within it, it's not a BLAS library?
        - Call `dpotrf` with a purposefully incorrect `lda` in order to get a negative return code of type `BlasInt`, and see if the result looks like a 32-bit or 64-bit error code.  [Example code here](https://github.com/JuliaLang/julia/blob/f8ff854e99f4a05a1abeafaac22ad88c77b1f677/stdlib/LinearAlgebra/src/blaslib.jl#L55-L95).



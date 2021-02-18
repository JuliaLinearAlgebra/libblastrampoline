# libblastrampoline

> All problems in computer science can be solved by another level of indirection

Using PLT trampolines to provide a BLAS and LAPACK demuxing library.

## Basic usage

Build `libblastrampoline.so`, then link your BLAS-using library against it instead of `libblas.so`.
When `libblastrampoline` is loaded, it will inspect the `LBT_DEFAULT_LIBS` environment variable and attempt to forward BLAS calls made to it on to that library (this can be a list of semicolon-separated libraries if your backing implementation is split across multiple libraries, such as in the case of separate `BLAS` and `LAPACK` libraries).
At any time, you may call `lbt_forward(libname, clear, verbose)` to redirect forwarding to a new BLAS library.
If you set `clear` to `1` it will clear out all previous mappings before setting new mappings, while if it is set to `0` it will leave symbols that do not exist within the given `libname` alone.
This is used to implement layering of libraries, such as between a split BLAS and LAPACK library:
```
lbt_forward("libblas.so", 1, 0);
lbt_forward("liblapack.so", 0, 0);
```

## ABI standard

`libblastrampoline` exports a consistent ABI for applications to link against.
In particular, we export both a 32-bit (LP64) and 64-bit (ILP64) interface, allowing applications that use one or the other (or both!) to link against the library.
Applications that wish to use the 64-bit interface must append `_64` to their function calls, e.g. instead of calling `dgemm()` they must call `dgemm_64()`.
The BLAS/LAPACK symbol list we re-export comes from the `gensymbol` script contained within `OpenBLAS`.
See [`ext/gensymbol`](ext/gensymbol) for more.
We note that we have an experimental `Clang.jl`-based symbol extractor that extracts only those symbols that are defined within the headers shipped with OpenBLAS, however as there are hundreds of symbols that `gensymbol` knows about (and are indeed exported from the shared library `libopenblas.so`) that are not included in the public C headers, we take the conservative approach and export the `gensymbol`-sourced symbols.

Because we export both the 32-bit (LP64) and 64-bit (ILP64) interfaces, if clients need header files defining the various BLAS/LAPACK functions, they must include headers defining the appropriate ABI.
We provide headers broken down by interface (`LP64` vs. `ILP64`) as well as target (e.g. `x86_64-linux-gnu`), so to properly compile your code with headers provided by `libblastrampoline` you must add the appropriate `-I${prefix}/include/${interface}/${target}` flags.

When `libblastrampoline` loads a BLAS/LAPACK library, it will inspect it to determine whether it is a 32-bit (LP64) or 64-bit (ILP64) library, and depending on the result, it will forward from its own 32-bit/64-bit names to the names declared in the library its forwarding to.  This allows automatic usage of multiple libraries with different interfaces but the same symbol names.

`libblastrampoline` is also cognizant of the f2c calling convention incompatibilities introduced by some libraries such as [Apple's Accelerate](https://developer.apple.com/documentation/accelerate).  It will automatically probe the library to determine its calling convention and employ a return-value conversion routine to fix the `float`/`double` return value differences.  This support is only available on the `x86_64` and `i686` architectures, however these are the only systems on which the incompatibilty exists to our knowledge.

### Version History

v1.1.0 - Added f2c autodetection for Accelerate.

v1.0.0 - Feburary 2021: Initial release with basic autodetection, LP64/ILP64 mixing and trampoline support.

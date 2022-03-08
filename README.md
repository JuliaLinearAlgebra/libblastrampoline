[![GitHub Actions CI](https://github.com/staticfloat/libblastrampoline/actions/workflows/ci.yml/badge.svg)](https://github.com/staticfloat/libblastrampoline/actions/workflows/ci.yml)
[![Drone Build Status](https://cloud.drone.io/api/badges/staticfloat/libblastrampoline/status.svg)](https://cloud.drone.io/staticfloat/libblastrampoline)

# libblastrampoline

> All problems in computer science can be solved by another level of indirection

Using [PLT trampolines](https://en.wikipedia.org/wiki/Trampoline_(computing)) to provide a BLAS and LAPACK demuxing library.

A detailed JuliaCon talk on libblastrampoline: https://www.youtube.com/watch?v=t6hptekOR7s

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
We note that we have an experimental [`Clang.jl`](https://github.com/JuliaInterop/Clang.jl)-based symbol extractor that extracts only those symbols that are defined within the headers shipped with OpenBLAS, however as there are hundreds of symbols that `gensymbol` knows about (and are indeed exported from the shared library `libopenblas.so`) that are not included in the public C headers, we take the conservative approach and export the `gensymbol`-sourced symbols.

Because we export both the 32-bit (LP64) and 64-bit (ILP64) interfaces, if clients need header files defining the various BLAS/LAPACK functions, they must include headers defining the appropriate ABI.
We provide headers broken down by interface (`LP64` vs. `ILP64`) as well as target (e.g. `x86_64-linux-gnu`), so to properly compile your code with headers provided by `libblastrampoline` you must add the appropriate `-I${prefix}/include/${interface}/${target}` flags.

When `libblastrampoline` loads a BLAS/LAPACK library, it will inspect it to determine whether it is a 32-bit (LP64) or 64-bit (ILP64) library, and depending on the result, it will forward from its own 32-bit/64-bit names to the names declared in the library its forwarding to.
This allows automatic usage of multiple libraries with different interfaces but the same symbol names.

`libblastrampoline` is also cognizant of the f2c calling convention incompatibilities introduced by some libraries such as [Apple's Accelerate](https://developer.apple.com/documentation/accelerate).
It will automatically probe the library to determine its calling convention and employ a return-value conversion routine to fix the `float`/`double` return value differences.
This support is only available on the `x86_64` and `i686` architectures, however these are the only systems on which the incompatibilty exists to our knowledge.

## `libblastrampoline`-specific API

`libblastrampoline` exports a simple configuration API including `lbt_forward()`, `lbt_get_config()`, `lbt_{set,get}_num_threads()`, and more.
Vendor-specific APIs such as `openblas_get_num_threads()` are not included in header files or exported from the library.
See the [public header file](src/libblastrampoline.h) for the most up-to-date documentation on the `libblastrampoline` API.

**Note**: all `lbt_*` functions should be considered thread-unsafe.
Do not attempt to load two BLAS libraries on two different threads at the same time.

### Limitations

This library has the ability to work with a mixture of LP64 and ILP64 BLAS libraries, but is slightly hampered on certain platforms that do not have the capability to perform `RTLD_DEEPBIND`-style linking.
As of the time of this writing, this includes FreeBSD and `musl` Linux.
The impact of this is that you are unable to load an ILP64 BLAS that exports the typical LP64 names (e.g. `dgemm_`) at the same time as an actual LP64 BLAS (with any naming scheme).
This is because without `RTLD_DEEPBIND`-style linking semantics, when the ILP64 BLAS tries to call one of its own functions, it will call the function exported by `libblastrampoline` itself, which will result in incorrect values and segfaults.
To address this, `libblastrampoline` will detect if you attempt to do this and refuse to load a library that would cause this kind of confusion.
You can always tell if your system is limited in this fashion by calling `lbt_get_config()` and checking the `build_flags` member for the `LBT_BUILDFLAGS_DEEPBINDLESS` flag.

### Version History

v5.0.2 - Add `cblas_sdot` and `cblas_ddot` to CBLAS divergence workaround wrappers.

v5.0.1 - Fix complex return wrapper infinite loop bug.

v5.0.0 - Add complex return value wrappers and CBLAS workaround.  The complex return value wrapper ensures that all symbols maintain a standard ABI for returning complex numbers, and the CBLAS workaround maps CBLAS symbols to FORTRAN symbols when properly-suffixed CBLAS symbols do not exist, as is the case in MKL `v2022.0`.

v4.1.0 - Add `LBT_STRICT` environment variable that causes calling missing symbols to kill the process.

v4.0.0 - Add `suffix_hint` parameter to `lbt_forward()` to allow overriding symbol suffix search order for dual-interface libraries, and allow loading the same library with multiple interfaces.

v3.1.0 - Add `LBT_USE_RTLD_DEEPBIND` environment variable override (for santizer usage), and add buildsystem fixes for Haiku.

v3.0.4 - Fix armv7l interface autodetection.

v3.0.3 - Fix armv7l trampolines from copy-paste error causing segfaults.

v3.0.2 - Fix MKL threading interface to use properly-capitalized names to get the C ABI.

v3.0.1 - Don't `dlclose()` libraries; this can cause crashes due to not knowing when resources are truly freed.

v3.0.0 - Added `active_forwards` field to `lbt_libinfo_t` and `exported_symbols` to `lbt_config_t`.

v2.2.0 - Removed useless `exit(1)` in `src/dl_utils.c`.

v2.1.0 - Added threading getter/setter API, direct setting API and default function API.

v2.0.0 - Added f2c autodetection for Accelerate, changed public API to `lbt_forward()` from `load_blas_funcs()`.

v1.0.0 - Feburary 2021: Initial release with basic autodetection, LP64/ILP64 mixing and trampoline support.

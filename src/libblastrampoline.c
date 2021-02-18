#include "libblastrampoline_internal.h"
#include "libblastrampoline_trampdata.h"

#ifdef F2C_AUTODETECTION
#include "libblastrampoline_f2cdata.h"
#endif

// Sentinel to tell us if we've got a deepbindless workaround active or not
#define DEEPBINDLESS_INTERFACE_LP64_LOADED    0x01
#define DEEPBINDLESS_INTERFACE_ILP64_LOADED   0x02
uint8_t deepbindless_interfaces_loaded      = 0x00;

/*
 * Load the given `libname`, lookup all registered symbols within our `exported_func_names` list,
 * and `dlsym()` the symbol addresses to load the addresses for forwarding into that library.
 *
 * If `clear` is set to a non-zero value, all symbol addresses will be NULL'ed out before they are
 * looked up in `libname`.  If `clear` is set to zero, symbols that do not exist in `libname` will
 * keep their previous value, which allows for loading a base library, then overriding some symbols
 * with a second shim library, integrating separate BLAS and LAPACK libraries, merging an LP64 and
 * ILP64 library into one, or all three use cases at the same time.
 *
 * Note that on certain platforms (currently musl linux and freebsd) you cannot load a non-suffixed
 * ILP64 and an LP64 BLAS at the same time.  Read the note below about lacking RTLD_DEEPBIND
 * support in the system libc for more details.
 *
 * If `verbose` is set to a non-zero value, it will print out debugging information.
 */
JL_DLLEXPORT int lbt_forward(const char * libname, int clear, int verbose) {
    if (verbose) {
        printf("Generating forwards to %s\n", libname);
    }

    // Load the library, throwing an error if we can't actually load it
    void * handle = load_library(libname);
    if (handle == NULL) {
        fprintf(stderr, "Unable to load \"%s\"\n", libname);
        return 0;
    }

    // Once we have the BLAS/LAPACK library loaded, we need to autodetect a few things about it.
    // First, we are going to figure out its name-mangling suffix:
    const char * lib_suffix = autodetect_symbol_suffix(handle);
    if (lib_suffix == NULL) {
        fprintf(stderr, "Unable to autodetect symbol suffix of \"%s\"\n", libname);
        return 0;
    }
    if (verbose) {
        printf(" -> Autodetected symbol suffix \"%s\"\n", lib_suffix);
    }

    // Next, we need to figure out if it's a 32-bit or 64-bit BLAS library;
    // we'll do that by calling `autodetect_interface()`:
    int interface = autodetect_interface(handle, lib_suffix);
    if (interface == 0) {
        fprintf(stderr, "Unable to autodetect interface type of \"%s\"\n", libname);
        return 0;
    }
    if (verbose) {
        if (interface == 64) {
            printf(" -> Autodetected interface ILP64 (64-bit)\n");
        } else {
            printf(" -> Autodetected interface LP64 (32-bit)\n");
        }
    }

#ifdef F2C_AUTODETECTION
    // Next, we need to probe to see if this is an f2c-style calling convention library
    // The only major example of this that we know of is Accelerate on macOS
    int f2c = autodetect_f2c(handle, lib_suffix);
    if (f2c == 0) {
        fprintf(stderr, "Unable to autodetect calling convention of \"%s\"\n", libname);
        return 0;
    }
    if (verbose) {
        if (f2c == 2) {
            printf(" -> Autodetected f2c-style calling convention\n");
        } else {
            printf(" -> Autodetected gfortran calling convention\n");
        }
    }
#endif

    /*
     * Now, if we are opening a 64-bit library with 32-bit names (e.g. suffix == ""),
     * we can handle that... as long as we're on a system where we can tell a library
     * to look up its own symbols before consulting the global symbol table.  This is
     * important so that when e.g. ILP64 `dgemm_` in this library wants to look up
     * `foo_`, it needs to find its own `foo_` but it will find the `foo_` trampoline
     * in this library unless we have `RTLD_DEEPBIND` semantics.  These semantics are
     * the default on MacOS and Windows, and on glibc Linux we enable it with the
     * dlopen flag `RTLD_DEEPBIND`, but on musl and FreeBSD we don't have access to
     * this flag, so we warn the user that they will be unable to load both LP64 and
     * ILP64 libraries on this system.  I hear support for this is coming in FreeBSD
     * 13.0, so some day this may be possible, but I sincerely hope that this
     * capability is not something being designed into new applications.
     *
     * If you are on a system without the ability for `RTLD_DEEPBIND` semantics no
     * sweat, this should work just fine as long as you either (a) only use one
     * BLAS library at a time, or (b) use two that have properly namespaced their
     * symbols with a different suffix.  But if you use two different BLAS libraries
     * with the same suffix, this library will complain.  Loudly.
     *
     * We track this by setting flags in `deepbindless_interfaces_loaded` to show
     * which interfaces have been loaded with an empty suffix; if the user
     * attempts to load another one without setting the `clear` flag, we refuse to
     * load it on a deepbindless system, printing out to `stderr` if we're verbose.
     */
#if !defined(RTLD_DEEPBIND) && (defined(_OS_LINUX_) || defined(_OS_FREEBSD_))
    // If `clear` is set, we clear our tracking
    if (clear) {
        deepbindless_interfaces_loaded = 0x00;
    }

    // If we ever load an LP64 BLAS, we mark that interface as being loaded since
    // we bind to the suffix-"" names, so even if the names of that library
    // internally are suffixed to something else, we ourselves will interfere with
    // a future suffix-"" ILP64 BLAS.
    if (interface == 32) {
        deepbindless_interfaces_loaded |= DEEPBINDLESS_INTERFACE_LP64_LOADED;
    }

    // We only mark a loaded ILP64 BLAS if it is a suffix-"" BLAS, since that is
    // the only case in which it will interfere with our LP64 BLAS symbols.
    if (lib_suffix[0] == '\0' && interface == 64) {
        deepbindless_interfaces_loaded |= DEEPBINDLESS_INTERFACE_ILP64_LOADED;
    }

    // If more than one flag is set, complain.
    if (deepbindless_interfaces_loaded == (DEEPBINDLESS_INTERFACE_ILP64_LOADED | DEEPBINDLESS_INTERFACE_LP64_LOADED)) {
        if (verbose) {
            fprintf(stderr, "ERROR: Cannot load both LP64 and ILP64 BLAS libraries without proper namespacing on an RTLD_DEEPBIND-less system!\n");
        }
        return 0;
    }
#endif

    // Finally, re-export its symbols:
    int nforwards = 0;
    int symbol_idx = 0;
    char symbol_name[MAX_SYMBOL_LEN];
    for (symbol_idx=0; exported_func_names[symbol_idx] != NULL; ++symbol_idx) {
        // If `clear` is set, zero out all symbols that may have been set so far
        if (clear) {
            (*exported_func32_addrs[symbol_idx]) = NULL;
            (*exported_func64_addrs[symbol_idx]) = NULL;
        }

        // Look up this symbol in the given library, if it is a valid symbol, set it!
        sprintf(symbol_name, "%s%s", exported_func_names[symbol_idx], lib_suffix);
        void *addr = lookup_symbol(handle, symbol_name);
        if (addr != NULL) {
            if (verbose) {
                char exported_name[MAX_SYMBOL_LEN];
                sprintf(exported_name, "%s%s", exported_func_names[symbol_idx], interface == 64 ? "64_" : "");
                printf(" - [%04d] %s -> %s [%p]\n", symbol_idx, exported_name, symbol_name, addr);
            }
            if (interface == 32) {
                (*exported_func32_addrs[symbol_idx]) = addr;
            } else {
                (*exported_func64_addrs[symbol_idx]) = addr;

                // If we're on an RTLD_DEEPBINDless system and our workaround is activated,
                // we take over our own 32-bit symbols as well.
                if (deepbindless_interfaces_loaded & DEEPBINDLESS_INTERFACE_ILP64_LOADED) {
                    (*exported_func32_addrs[symbol_idx]) = addr;
                }
            }
            nforwards++;
        }
    }

    if (verbose) {
        printf("Processed %d symbols; forwarded %d symbols with %d-bit interface and mangling to a suffix of \"%s\"\n", symbol_idx, nforwards, interface, lib_suffix);
    }

#ifdef F2C_AUTODETECTION
    if (f2c == 2) {
        int f2c_symbol_idx = 0;
        for (f2c_symbol_idx=0; f2c_func_idxs[f2c_symbol_idx] != -1; ++f2c_symbol_idx) {
            // Jump through the f2c_func_idxs layer of indirection to find the `exported_func*_addrs` offsets
            symbol_idx = f2c_func_idxs[f2c_symbol_idx];

            if (verbose) {
                char exported_name[MAX_SYMBOL_LEN];
                sprintf(exported_name, "%s%s", exported_func_names[symbol_idx], interface == 64 ? "64_" : "");
                printf(" - [%04d] f2c(%s)\n", symbol_idx, exported_name);
            }

            // Override these addresses with our f2c wrappers
            if (interface == 32) {
                // Save "true" symbol address in `f2c_$(name)_addr`, then set our exported `$(name)` symbol
                // to call `f2c_$(name)`, which will bounce into the true symbol, but fix the return value.
                (*f2c_func32_addrs[f2c_symbol_idx]) = (*exported_func32_addrs[symbol_idx]);
                (*exported_func32_addrs[symbol_idx]) = f2c_func32_wrappers[f2c_symbol_idx];
            } else {
                (*f2c_func64_addrs[f2c_symbol_idx]) = (*exported_func64_addrs[symbol_idx]);
                (*exported_func64_addrs[symbol_idx]) = f2c_func64_wrappers[f2c_symbol_idx];
            }
        }
    }
#endif

    return nforwards;
}

__attribute__((constructor)) void init(void) {
    // If LBT_VERBOSE == "1", the startup invocation should be verbose
    int verbose = 0;
    const char * verbose_str = getenv("LBT_VERBOSE");
    if (verbose_str != NULL && strcmp(verbose_str, "1") == 0) {
        verbose = 1;
        printf("libblastrampoline initializing\n");
    }

    // LBT_DEFAULT_LIBS is a semicolon-separated list of paths that should be loaded as BLAS libraries
    const char * default_libs = getenv("LBT_DEFAULT_LIBS");
    if (default_libs != NULL) {
        const char * curr_lib_start = default_libs;
        int clear = 1;
        char curr_lib[PATH_MAX];
        while (curr_lib_start[0] != '\0') {
            // Find the end of this current library name
            const char * end = curr_lib_start;
            while (*end != ';' && *end != '\0')
                end++;

            // Copy it into a temporary location
            int len = end - curr_lib_start;
            memcpy(curr_lib, curr_lib_start, len);
            curr_lib[len] = '\0';
            curr_lib_start = end;
            while (curr_lib_start[0] == ';')
                curr_lib_start++;

            // Load functions from this library, clearing only the first time.
            lbt_forward(curr_lib, clear, verbose);
            clear = 0;
        }
    }
}

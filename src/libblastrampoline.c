#include "libblastrampoline_internal.h"
#include "libblastrampoline_trampdata.h"

#ifdef COMPLEX_RETSTYLE_AUTODETECTION
#include "libblastrampoline_complex_retdata.h"
#endif
#ifdef F2C_AUTODETECTION
#include "libblastrampoline_f2cdata.h"
#endif
#ifdef CBLAS_DIVERGENCE_AUTODETECTION
#include "libblastrampoline_cblasdata.h"
#endif

// Sentinel to tell us if we've got a deepbindless workaround active or not
#define DEEPBINDLESS_INTERFACE_LP64_LOADED    0x01
#define DEEPBINDLESS_INTERFACE_ILP64_LOADED   0x02
uint8_t deepbindless_interfaces_loaded      = 0x00;


int32_t find_symbol_idx(const char * name) {
    for (int32_t symbol_idx=0; exported_func_names[symbol_idx] != NULL; ++symbol_idx) {
        if (strcmp(exported_func_names[symbol_idx], name) == 0) {
            return symbol_idx;
        }
    }
    return -1;
}


LBT_DLLEXPORT void lbt_default_func_print_error() {
    fprintf(stderr, "Error: no BLAS/LAPACK library loaded!\n");
}
void lbt_default_func_print_error_and_exit() {
    lbt_default_func_print_error();
    exit(1);
}
const void * default_func = (const void *)&lbt_default_func_print_error;
LBT_DLLEXPORT const void * lbt_get_default_func() {
    return default_func;
}

LBT_DLLEXPORT void lbt_set_default_func(const void * addr) {
    default_func = addr;
}

/*
 * Force a forward to a particular value.
 */
int32_t set_forward_by_index(int32_t symbol_idx, const void * addr, int32_t interface, int32_t complex_retstyle, int32_t f2c, int32_t verbose) {
    // Quit out immediately if this is not a interface setting
    if (interface != LBT_INTERFACE_LP64 && interface != LBT_INTERFACE_ILP64) {
        return -1;
    }

    // NULL is a special value that means our "default address"... which may itself be `NULL`!
    if (addr == NULL) {
        addr = default_func;
    }

    if (interface == LBT_INTERFACE_LP64) {
        (*exported_func32_addrs[symbol_idx]) = addr;
    } else {
        (*exported_func64_addrs[symbol_idx]) = addr;

        // If we're on an RTLD_DEEPBIND-less system and our workaround is activated,
        // we take over our own 32-bit symbols as well.
        if (deepbindless_interfaces_loaded & DEEPBINDLESS_INTERFACE_ILP64_LOADED) {
            (*exported_func32_addrs[symbol_idx]) = addr;
        }
    }

#ifdef COMPLEX_RETSTYLE_AUTODETECTION
    if (complex_retstyle == LBT_COMPLEX_RETSTYLE_ARGUMENT) {
        // Check to see if this symbol is one of the complex-returning functions
        for (int complex_symbol_idx=0; cmplxret_func_idxs[complex_symbol_idx] != -1; ++complex_symbol_idx) {
            // Skip any symbols that aren't ours
            if (cmplxret_func_idxs[complex_symbol_idx] != symbol_idx)
                continue;

            // Report to the user that we're cblas-wrapping this one
            if (verbose) {
                char exported_name[MAX_SYMBOL_LEN];
                sprintf(exported_name, "%s%s", exported_func_names[symbol_idx], interface == LBT_INTERFACE_ILP64 ? "64_" : "");
                printf(" - [%04d] complex(%s)\n", symbol_idx, exported_name);
            }

            if (interface == LBT_INTERFACE_LP64) {
                (*cmplxret_func32_addrs[complex_symbol_idx]) = (*exported_func32_addrs[symbol_idx]);
                (*exported_func32_addrs[symbol_idx]) = cmplxret32_func_wrappers[complex_symbol_idx];
            } else {
                (*cmplxret_func64_addrs[complex_symbol_idx]) = (*exported_func64_addrs[symbol_idx]);
                (*exported_func64_addrs[symbol_idx]) = cmplxret64_func_wrappers[complex_symbol_idx];
            }
        }
    }
#endif // COMPLEX_RETSTYLE_AUTODETECTION

#ifdef F2C_AUTODETECTION
    if (f2c == LBT_F2C_REQUIRED) {
        // Check to see if this symbol is one of the f2c functions
        for (int f2c_symbol_idx=0; f2c_func_idxs[f2c_symbol_idx] != -1; ++f2c_symbol_idx) {
            // Jump through the f2c_func_idxs layer of indirection to find the `exported_func*_addrs` offsets
            // Skip any symbols that aren't ours
            if (f2c_func_idxs[f2c_symbol_idx] != symbol_idx)
                continue;

            if (verbose) {
                char exported_name[MAX_SYMBOL_LEN];
                sprintf(exported_name, "%s%s", exported_func_names[symbol_idx], interface == LBT_INTERFACE_ILP64 ? "64_" : "");
                printf(" - [%04d] f2c(%s)\n", symbol_idx, exported_name);
            }

            // Override these addresses with our f2c wrappers
            if (interface == LBT_INTERFACE_LP64) {
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
#endif // F2C_AUTODETECTION
    return 0;
}

LBT_DLLEXPORT const void * lbt_get_forward(const char * symbol_name, int32_t interface, int32_t f2c) {
    // Search symbol list for `symbol_name`, then sub off to `set_forward_by_index()`
    int32_t symbol_idx = find_symbol_idx(symbol_name);
    if (symbol_idx == -1)
        return (const void *)-1;
    
#ifdef F2C_AUTODETECTION
    if (f2c == LBT_F2C_REQUIRED) {
        // Check to see if this symbol is one of the f2c functions
        int f2c_symbol_idx = 0;
        for (f2c_symbol_idx=0; f2c_func_idxs[f2c_symbol_idx] != -1; ++f2c_symbol_idx) {
            // Skip any symbols that aren't ours
            if (f2c_func_idxs[f2c_symbol_idx] != symbol_idx)
                continue;

            // If we find it, return the "true" address, but only if the currently-exported
            // address is actually our f2c wrapper; if it's not then do nothing.
            if (interface == LBT_INTERFACE_LP64) {
                if (*exported_func32_addrs[symbol_idx] == f2c_func32_wrappers[f2c_symbol_idx]) {
                    return (const void *)(*f2c_func32_addrs[f2c_symbol_idx]);
                }
            } else {
                if (*exported_func64_addrs[symbol_idx] == f2c_func64_wrappers[f2c_symbol_idx]) {
                    return (const void *)(*f2c_func64_addrs[f2c_symbol_idx]);
                }
            }
        }
    }
#endif

    // If we're not in f2c-hell, we can just return our interface's address directly.
    if (interface == LBT_INTERFACE_LP64) {
        return (const void *)(*exported_func32_addrs[symbol_idx]);
    } else {
        return (const void *)(*exported_func64_addrs[symbol_idx]);
    }
}

LBT_DLLEXPORT int32_t lbt_set_forward(const char * symbol_name, const void * addr, int32_t interface, int32_t complex_retstyle, int32_t f2c, int32_t verbose) {
    // Search symbol list for `symbol_name`, then sub off to `set_forward_by_index()`
    int32_t symbol_idx = find_symbol_idx(symbol_name);
    if (symbol_idx == -1)
        return -1;

    int32_t ret = set_forward_by_index(symbol_idx, addr, interface, complex_retstyle, f2c, verbose);
    if (ret == 0) {
        // Un-mark this symbol as being provided by any of our libraries;
        // if you use the footgun API, you can keep track of who is providing what.
        clear_forwarding_mark(symbol_idx, interface);
    }
    return ret;
}

// Load `libname`, clearing previous mappings if `clear` is set.
LBT_DLLEXPORT int32_t lbt_forward(const char * libname, int32_t clear, int32_t verbose, const char * suffix_hint) {
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
    const char * lib_suffix = autodetect_symbol_suffix(handle, suffix_hint);
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
    if (interface == LBT_INTERFACE_UNKNOWN) {
        fprintf(stderr, "Unable to autodetect interface type of \"%s\"\n", libname);
        return 0;
    }
    if (verbose) {
        if (interface == LBT_INTERFACE_ILP64) {
            printf(" -> Autodetected interface ILP64 (64-bit)\n");
        }
        if (interface == LBT_INTERFACE_LP64) {
            printf(" -> Autodetected interface LP64 (32-bit)\n");
        }
    }

    // Next, let's figure out what the complex return style is:
    int complex_retstyle = LBT_COMPLEX_RETSTYLE_UNKNOWN;
#ifdef COMPLEX_RETSTYLE_AUTODETECTION
    complex_retstyle = autodetect_complex_return_style(handle, lib_suffix);
    if (complex_retstyle == LBT_COMPLEX_RETSTYLE_UNKNOWN) {
        fprintf(stderr, "Unable to autodetect complex return style of \"%s\"\n", libname);
        return 0;
    }
    if (verbose) {
        if (complex_retstyle == LBT_COMPLEX_RETSTYLE_NORMAL) {
            printf(" -> Autodetected normal complex return style\n");
        }
        if (complex_retstyle == LBT_COMPLEX_RETSTYLE_ARGUMENT) {
            printf(" -> Autodetected argument-passing complex return style\n");
        }
    }
#endif // COMPLEX_RETSTYLE_AUTODETECTION

    int f2c = LBT_F2C_PLAIN;
#ifdef F2C_AUTODETECTION
    // Next, we need to probe to see if this is an f2c-style calling convention library
    // The only major example of this that we know of is Accelerate on macOS
    f2c = autodetect_f2c(handle, lib_suffix);
    if (f2c == LBT_F2C_UNKNOWN) {
        fprintf(stderr, "Unable to autodetect calling convention of \"%s\"\n", libname);
        return 0;
    }
    if (verbose) {
        if (f2c == LBT_F2C_REQUIRED) {
            printf(" -> Autodetected f2c-style calling convention\n");
        }
        if (f2c == LBT_F2C_PLAIN) {
            printf(" -> Autodetected gfortran calling convention\n");
        }
    }
#endif // F2C_AUTODETECTION

    int cblas = LBT_CBLAS_UNKNOWN;
#ifdef CBLAS_DIVERGENCE_AUTODETECTION
    // Next, we need to probe to see if this is MKL v2022 with missing ILP64-suffixed
    // CBLAS symbols, but only if it's an ILP64 library.
    if (interface == LBT_INTERFACE_ILP64) {
        cblas = autodetect_cblas_divergence(handle, lib_suffix);
        if (verbose) {
            switch (cblas) {
                case LBT_CBLAS_CONFORMANT:
                    printf(" -> CBLAS detected\n");
                    break;
                case LBT_CBLAS_DIVERGENT:
                    printf(" -> Autodetected CBLAS-divergent library!\n");
                    break;
                case LBT_CBLAS_UNKNOWN:
                    printf(" -> CBLAS not found\n");
                    break;
                default:
                    printf(" -> ERROR: Impossible CBLAS detection result: %d\n", cblas);
                    cblas = LBT_CBLAS_UNKNOWN;
                    break;
            }
        }
    }
#endif // CBLAS_DIVERGENCE_AUTODETECTION

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

    // If `clear` is set, we clear our tracking
    if (clear) {
        deepbindless_interfaces_loaded = 0x00;
    }

    // If we ever load an LP64 BLAS, we mark that interface as being loaded since
    // we bind to the suffix-"" names, so even if the names of that library
    // internally are suffixed to something else, we ourselves will interfere with
    // a future suffix-"" ILP64 BLAS.
    if ((use_deepbind == 0x00) && (interface == LBT_INTERFACE_LP64)) {
        deepbindless_interfaces_loaded |= DEEPBINDLESS_INTERFACE_LP64_LOADED;
    }

    // We only mark a loaded ILP64 BLAS if it is a suffix-"" BLAS, since that is
    // the only case in which it will interfere with our LP64 BLAS symbols.
    if ((use_deepbind == 0x00) && (lib_suffix[0] == '\0' && interface == LBT_INTERFACE_ILP64)) {
        deepbindless_interfaces_loaded |= DEEPBINDLESS_INTERFACE_ILP64_LOADED;
    }

    // If more than one flag is set, complain.
    if (deepbindless_interfaces_loaded == (DEEPBINDLESS_INTERFACE_ILP64_LOADED | DEEPBINDLESS_INTERFACE_LP64_LOADED)) {
        if (verbose) {
            fprintf(stderr, "ERROR: Cannot load both LP64 and ILP64 BLAS libraries without proper namespacing on an RTLD_DEEPBIND-less system!\n");
        }
        return 0;
    }

    // If `clear` is set, drop all information about previously-loaded libraries
    if (clear) {
        clear_loaded_libraries();
    }

    // Finally, re-export its symbols:
    int32_t nforwards = 0;
    int32_t symbol_idx = 0;
    char symbol_name[MAX_SYMBOL_LEN];
    uint8_t forwards[(NUM_EXPORTED_FUNCS/8) + 1] = {0};
    for (symbol_idx=0; exported_func_names[symbol_idx] != NULL; ++symbol_idx) {
        // If `clear` is set, zero out all symbols that may have been set so far
        if (clear) {
            (*exported_func32_addrs[symbol_idx]) = default_func;
            (*exported_func64_addrs[symbol_idx]) = default_func;
        }

        // Look up this symbol in the given library, if it is a valid symbol, set it!
        sprintf(symbol_name, "%s%s", exported_func_names[symbol_idx], lib_suffix);
        void *addr = lookup_symbol(handle, symbol_name);
        if (addr != NULL) {
            set_forward_by_index(symbol_idx,  addr, interface, complex_retstyle, f2c, verbose);
            BITFIELD_SET(forwards, symbol_idx);
            nforwards++;
        }
    }

#ifdef CBLAS_DIVERGENCE_AUTODETECTION
    // If we're loading a divergent CBLAS library, we need to scan through all
    // CBLAS symbols, and forward them to wrappers which will convert them to
    // the FORTRAN equivalents.
    if (cblas == LBT_CBLAS_DIVERGENT) {
        int32_t cblas_symbol_idx = 0;
        for (cblas_symbol_idx = 0; cblas_func_idxs[cblas_symbol_idx] != -1; cblas_symbol_idx += 1) {
            int32_t symbol_idx = cblas_func_idxs[cblas_symbol_idx];

            // Report to the user that we're cblas-wrapping this one
            if (verbose) {
                char exported_name[MAX_SYMBOL_LEN];
                sprintf(exported_name, "%s%s", exported_func_names[symbol_idx], interface == LBT_INTERFACE_ILP64 ? "64_" : "");
                printf(" - [%04d] cblas(%s)\n", symbol_idx, exported_name);
            }

            if (interface == LBT_INTERFACE_LP64) {
                (*exported_func32_addrs[symbol_idx]) = cblas32_func_wrappers[cblas_symbol_idx];
            } else {
                (*exported_func64_addrs[symbol_idx]) = cblas64_func_wrappers[cblas_symbol_idx];
            }
        }
    }
#endif // CBLAS_DIVERGENCE_AUTODETECTION

    record_library_load(libname, handle, lib_suffix, &forwards[0], interface, complex_retstyle, f2c, cblas);
    if (verbose) {
        printf("Processed %d symbols; forwarded %d symbols with %d-bit interface and mangling to a suffix of \"%s\"\n", symbol_idx, nforwards, interface, lib_suffix);
    }

    return nforwards;
}

/*
 * On windows it's surprisingly difficult to get a handle to ourselves,
 * and that's because they give it to you in `DllMain()`.  ;)
 */
#ifdef _OS_WINDOWS_
void * _win32_self_handle;
BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD code, void *reserved) {
    if (code == DLL_PROCESS_ATTACH) {
        _win32_self_handle = (void *)hModule;
    }
#else
__attribute__((constructor)) void init(void) {
#endif
    // Initialize config structures
    init_config();

    // If LBT_VERBOSE == "1", the startup invocation should be verbose
    int verbose = 0;
    const char * verbose_str = getenv("LBT_VERBOSE");
    if (verbose_str != NULL && strcmp(verbose_str, "1") == 0) {
        verbose = 1;
        printf("libblastrampoline initializing from %s\n", lookup_self_path());
    }

#if !defined(LBT_DEEPBINDLESS)
    // If LBT_USE_RTLD_DEEPBIND == "0", we avoid using RTLD_DEEPBIND on a
    // deepbind-capable system.  This is mostly useful for sanitizers, which
    // abhor such library loading shenanigans.
    const char * deepbindless_str = getenv("LBT_USE_RTLD_DEEPBIND");
    if (deepbindless_str != NULL && strcmp(deepbindless_str, "0") == 0) {
        use_deepbind = 0x00;
        if (verbose) {
            printf("LBT_USE_RTLD_DEEPBIND=0 detected; avoiding usage of RTLD_DEEPBIND\n");
        }
    }
#endif // !defined(LBT_DEEPBINDLESS)

    const char * strict_str = getenv("LBT_STRICT");
    if (strict_str != NULL && strcmp(strict_str, "1") == 0) {
        if (verbose) {
            printf("LBT_STRICT=1 detected; calling missing symbols will print an error, then exit\n");
        }
        // We can't directly use the symbol name here, since the protected visibility
        // on Linux causes a linker error with certain versions of GCC and ld:
        // https://lists.gnu.org/archive/html/bug-binutils/2016-02/msg00191.html
        default_func = lookup_self_symbol("lbt_default_func_print_error_and_exit");
    }

    // LBT_DEFAULT_LIBS is a semicolon-separated list of paths that should be loaded as BLAS libraries
    const char * default_libs = getenv("LBT_DEFAULT_LIBS");
#if defined(LBT_FALLBACK_LIBS)
    if (default_libs == NULL) {
        default_libs = LBT_FALLBACK_LIBS;
    }
#endif
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
            lbt_forward(curr_lib, clear, verbose, NULL);
            clear = 0;
        }
    }

#ifdef _OS_WINDOWS_
    return TRUE;
#endif
}

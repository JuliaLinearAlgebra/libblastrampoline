#include "libblastrampoline_internal.h"
#include "libblastrampoline_trampdata.h"

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
 * If `verbose` is set to a non-zero value, it will print out debugging information.
 */
JL_DLLEXPORT int load_blas_funcs(const char * libname, int clear, int verbose) {
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
            }
            nforwards++;
        }
    }

    if (verbose) {
        printf("Processed %d symbols; forwarded %d symbols with %d-bit interface and mangling to a suffix of \"%s\"\n", symbol_idx, nforwards, interface, lib_suffix);
    }
    return nforwards;
}

__attribute__((constructor)) void init(void) {
    // If LIBBLAS_VERBOSE == "1", the startup invocation should be verbose
    int verbose = 0;
    const char * verbose_str = getenv("LBT_VERBOSE");
    if (verbose_str != NULL && strcmp(verbose_str, "1") == 0) {
        verbose = 1;
    }

    const char * default_libs = getenv("LBT_DEFAULT_LIBS");
    if (default_libs != NULL) {
        const char * curr_lib_start = default_libs;
        int clear = 1;
        char curr_lib[PATH_MAX];
        while (curr_lib_start[0] != '\0') {
            // Find the end of this current library name
            const char * end = curr_lib_start;
            while (*end != ':' && *end != '\0')
                end++;

            // Copy it into a temporary location
            int len = end - curr_lib_start;
            memcpy(curr_lib, curr_lib_start, len);
            curr_lib[len] = '\0';
            curr_lib_start = end;
            while (curr_lib_start[0] == ':')
                curr_lib_start++;

            // Load functions from this library, clearing only the first time.
            load_blas_funcs(curr_lib, clear, verbose);
            clear = 0;
        }
    }
}

#include "libblastrampoline_internal.h"

#define MAX_THREADING_NAMES     32

#ifndef max
#define max(x, y)  ((x) > (y) ? (x) : (y))
#endif

/* We need to ask MKL to get/set threads for only the BLAS & LAPACK domains, we
 * therefore pass in this constant to the relevant threading functions to limit
 * our thread setting.
 *
 * The LAPACK threading domain was added in 2025.2, so we need to ask for the MKL
 * version in order to properly handle that.
 *
 * These are from mkl_types.h
 */
#define MKL_DOMAIN_BLAS   1
#define MKL_DOMAIN_LAPACK 5

 typedef
 struct {
     int    MajorVersion;
     int    MinorVersion;
     int    UpdateVersion;
     int    PatchVersion;
     char * ProductStatus;
     char * Build;
     char * Processor;
     char * Platform;
 } MKLVersion;


/*
 * We provide a flexible thread getter/setter interface here; by calling `lbt_set_num_threads()`
 * libblastrampoline will propagate the call through to its loaded libraries as long as the
 * library exposes a getter/setter method named in one of these arrays, and its signature
 * matches the common theme here.
 */
static char * getter_names[MAX_THREADING_NAMES] = {
    "openblas_get_num_threads",
    "bli_thread_get_num_threads",
    // We special-case MKL in the lookup loop below
    //"MKL_Domain_Get_Max_Threads",
    NULL
};

static char * setter_names[MAX_THREADING_NAMES] = {
    "openblas_set_num_threads",
    "bli_thread_set_num_threads",
    // We special-case MKL in the lookup loop below
    //"MKL_Domain_Set_Num_Threads",
    NULL
};

/*
 * If you have a truly custom BLAS, you can pass in the explicit getter/setter method names here.
 * Note that these names will have the library suffix appended to them!
 */
LBT_DLLEXPORT void lbt_register_thread_interface(const char * getter, const char * setter) {
    int idx = 0;
    while (getter_names[idx] != NULL) {
        // We refuse to register ridiculous amounts of these
        if (idx >= MAX_THREADING_NAMES - 1) {
            return;
        }
        // We don't allow duplicates
        if (strcmp(getter_names[idx], getter) == 0 && strcmp(setter_names[idx], setter) == 0) {
            return;
        }
        idx++;
    }

    getter_names[idx] = strdup(getter);
    setter_names[idx] = strdup(setter);
}

/*
 * Returns the number of threads configured in all loaded libraries.
 * In the event of a mismatch, returns the largest value.
 * If no BLAS libraries with a known threading interface are loaded,
 * returns `1`.
 */
LBT_DLLEXPORT int32_t lbt_get_num_threads() {
    int32_t max_threads = 1;

    const lbt_config_t * config = lbt_get_config();
    for (int lib_idx=0; config->loaded_libs[lib_idx] != NULL; ++lib_idx) {
        lbt_library_info_t * lib = config->loaded_libs[lib_idx];
        for (int symbol_idx=0; getter_names[symbol_idx] != NULL; ++symbol_idx) {
            char symbol_name[MAX_SYMBOL_LEN];
            build_symbol_name(symbol_name, getter_names[symbol_idx], lib->suffix);
            int (*fptr)() = lookup_symbol(lib->handle, symbol_name);
            if (fptr != NULL) {
                int new_threads = fptr();
                max_threads = max(max_threads, new_threads);
            }
        }

        // Special-case MKL, as we need to specifically ask for the BLAS & LAPACK domains
        int (*fptr)(int) = lookup_symbol(lib->handle, "MKL_Domain_Get_Max_Threads");
        if (fptr != NULL) {
            // The BLAS domain (always available)
            int new_threads_blas = fptr(MKL_DOMAIN_BLAS);
            max_threads = max(max_threads, new_threads_blas);

            // The LAPACK threading domain was only added in oneMKL 2025.2
            // Gate reading the threads based on this version, because before 2025.2 it
            // will return the default, which wouldn't be useful because we max everything.
            void (*fverptr)(MKLVersion*) = lookup_symbol(lib->handle, "mkl_get_version");
            if (fverptr != NULL) {
                MKLVersion ver;
                fverptr(&ver);

                // MKL considers 2025.2 to be a major of 2025 and an update of 2 (not a minor)
                if(ver.MajorVersion >= 2025 && ver.UpdateVersion >= 2) {
                    int new_threads_lapack = fptr(MKL_DOMAIN_LAPACK);
                    max_threads = max(max_threads, new_threads_lapack);
                }
            }
        }
    }
    return max_threads;
}



/*
 * Sets the given number of threads for all loaded libraries.
 */
LBT_DLLEXPORT void lbt_set_num_threads(int32_t nthreads) {
    const lbt_config_t * config = lbt_get_config();
    char symbol_name[MAX_SYMBOL_LEN];
    for (int lib_idx=0; config->loaded_libs[lib_idx] != NULL; ++lib_idx) {
        lbt_library_info_t * lib = config->loaded_libs[lib_idx];
        for (int symbol_idx=0; setter_names[symbol_idx] != NULL; ++symbol_idx) {
            build_symbol_name(symbol_name, setter_names[symbol_idx], lib->suffix);
            void (*fptr)(int) = lookup_symbol(lib->handle, symbol_name);
            if (fptr != NULL) {
                fptr(nthreads);
            }
        }

        // Special-case MKL, as we need to specifically ask for the BLAS & LAPACK domains
        int (*fptr)(int, int) = lookup_symbol(lib->handle, "MKL_Domain_Set_Num_Threads");
        if (fptr != NULL) {
            fptr(nthreads, MKL_DOMAIN_BLAS);
            fptr(nthreads, MKL_DOMAIN_LAPACK);
        }
    }
}

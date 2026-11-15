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


/* Apple Accelerate doesn't allow setting the number of threads directly, it only has an
 * option to do single-threaded or multi-threaded. That is controlled via the BLASSetThreading
 * API introduced in macOS 15.
 *
 * These constants are from the vecLib.framework/Headers/thread_api.h file
 */
#define ACCELERATE_BLAS_THREADING_MULTI_THREADED    0
#define ACCELERATE_BLAS_THREADING_SINGLE_THREADED   1

/*
 * We provide a flexible thread getter/setter interface here; by calling `lbt_set_num_threads()`
 * libblastrampoline will propagate the call through to its loaded libraries as long as the
 * library exposes a getter/setter method named in one of these arrays, and its signature
 * matches the common theme here.
 *
 * For the getters, the function signature should match:
 *     int get_num_threads()
 * For the setters, the function signature should match:
 *     void set_num_threads(int numThreads)
 */
static char * getter_names[MAX_THREADING_NAMES] = {
    "openblas_get_num_threads",
    "bli_thread_get_num_threads",
    // There isn't a non-max NVPL function for this
    "nvpl_blas_get_max_threads",
    "nvpl_lapack_get_max_threads",
    // We special-case MKL in the lookup loop below
    //"MKL_Domain_Get_Max_Threads",
    // We special-case Apple Accelerate below
    NULL
};

static char * setter_names[MAX_THREADING_NAMES] = {
    "openblas_set_num_threads",
    "bli_thread_set_num_threads",
    "nvpl_blas_set_num_threads",
    "nvpl_lapack_set_num_threads",
    // We special-case MKL in the lookup loop below
    //"MKL_Domain_Set_Num_Threads",
    // We special-case Apple Accelerate below
    NULL
};

/*
 * Resolving the getters/setters above requires a `dlsym()` per name per library, and failed
 * lookups can be very slow (tens of microseconds each on macOS).  Since `lbt_get_num_threads()`
 * may be called very frequently (e.g. before every BLAS call to pick a code path), we resolve
 * the function pointers once, whenever the set of loaded libraries or the set of registered
 * threading names changes, and cache them here.  `lbt_get_num_threads()` and
 * `lbt_set_num_threads()` then only perform direct calls through these cached pointers.
 *
 * The cache is rebuilt by `refresh_threading_cache()`, which is called from
 * `record_library_load()`, `clear_loaded_libraries()` and `lbt_register_thread_interface()`.
 */
typedef struct {
    int num_getters;
    int (*getters[MAX_THREADING_NAMES])(void);
    int num_setters;
    void (*setters[MAX_THREADING_NAMES])(int);

    // MKL
    int (*mkl_get)(int);
    int (*mkl_set)(int, int);
    // Whether MKL supports the LAPACK threading domain (oneMKL 2025.2+)
    int mkl_has_lapack_domain;

    // Apple Accelerate
    int (*acc_get)(void);
    int (*acc_nthreads)(void);
    int (*acc_set)(int);
} lbt_threading_info_t;

static lbt_threading_info_t threading_cache[MAX_TRACKED_LIBS];
static int threading_cache_num_libs = 0;

static void resolve_threading_info(lbt_threading_info_t * info, const lbt_library_info_t * lib) {
    char symbol_name[MAX_SYMBOL_LEN];
    memset(info, 0, sizeof(lbt_threading_info_t));

    for (int symbol_idx=0; getter_names[symbol_idx] != NULL; ++symbol_idx) {
        build_symbol_name(symbol_name, getter_names[symbol_idx], lib->suffix);
        int (*fptr)(void) = lookup_symbol(lib->handle, symbol_name);
        if (fptr != NULL) {
            info->getters[info->num_getters++] = fptr;
        }
    }
    for (int symbol_idx=0; setter_names[symbol_idx] != NULL; ++symbol_idx) {
        build_symbol_name(symbol_name, setter_names[symbol_idx], lib->suffix);
        void (*fptr)(int) = lookup_symbol(lib->handle, symbol_name);
        if (fptr != NULL) {
            info->setters[info->num_setters++] = fptr;
        }
    }

    // Special-case MKL, as we need to specifically ask for the BLAS & LAPACK domains
    info->mkl_get = lookup_symbol(lib->handle, "MKL_Domain_Get_Max_Threads");
    info->mkl_set = lookup_symbol(lib->handle, "MKL_Domain_Set_Num_Threads");
    if (info->mkl_get != NULL) {
        // The LAPACK threading domain was only added in oneMKL 2025.2
        // Gate reading the threads based on this version, because before 2025.2 it
        // will return the default, which wouldn't be useful because we max everything.
        void (*fverptr)(MKLVersion*) = lookup_symbol(lib->handle, "mkl_get_version");
        if (fverptr != NULL) {
            MKLVersion ver;
            fverptr(&ver);

            // MKL considers 2025.2 to be a major of 2025 and an update of 2 (not a minor)
            if (ver.MajorVersion >= 2025 && ver.UpdateVersion >= 2) {
                info->mkl_has_lapack_domain = 1;
            }
        }
    }

    // Special case Apple Accelerate; these APIs only exist on macOS 15+.
    info->acc_get = lookup_symbol(lib->handle, "BLASGetThreading");
    if (info->acc_get != NULL) {
        info->acc_nthreads = lookup_symbol(lib->handle, "APPLE_NTHREADS");
    }
    info->acc_set = lookup_symbol(lib->handle, "BLASSetThreading");
}

void refresh_threading_cache() {
    // Mark the cache as empty while we rebuild it, so that a concurrent reader skips
    // entries rather than calling through half-written ones.
    threading_cache_num_libs = 0;

    int num_libs = 0;
    const lbt_config_t * config = lbt_get_config();
    for (int lib_idx=0; lib_idx < MAX_TRACKED_LIBS && config->loaded_libs[lib_idx] != NULL; ++lib_idx) {
        resolve_threading_info(&threading_cache[num_libs], config->loaded_libs[lib_idx]);
        num_libs++;
    }
    threading_cache_num_libs = num_libs;
}

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

    // Pick up the new getter/setter in any already-loaded libraries
    refresh_threading_cache();
}

/*
 * Returns the number of threads configured in all loaded libraries.
 * In the event of a mismatch, returns the largest value.
 * If no BLAS libraries with a known threading interface are loaded,
 * returns `1`.
 */
LBT_DLLEXPORT int32_t lbt_get_num_threads() {
    int32_t max_threads = 1;

    const int num_libs = threading_cache_num_libs;
    for (int lib_idx=0; lib_idx < num_libs; ++lib_idx) {
        const lbt_threading_info_t * info = &threading_cache[lib_idx];
        for (int idx=0; idx < info->num_getters; ++idx) {
            int new_threads = info->getters[idx]();
            max_threads = max(max_threads, new_threads);
        }

        // Special-case MKL, as we need to specifically ask for the BLAS & LAPACK domains
        if (info->mkl_get != NULL) {
            // The BLAS domain (always available)
            int new_threads_blas = info->mkl_get(MKL_DOMAIN_BLAS);
            max_threads = max(max_threads, new_threads_blas);

            if (info->mkl_has_lapack_domain) {
                int new_threads_lapack = info->mkl_get(MKL_DOMAIN_LAPACK);
                max_threads = max(max_threads, new_threads_lapack);
            }
        }

        // Special case Apple Accelerate because we have to determine if we are single-threaded or multi-threaded
        // This API only exists on macOS 15+.
        if (info->acc_get != NULL) {
            int threading = info->acc_get();

            if (threading == ACCELERATE_BLAS_THREADING_MULTI_THREADED) {
                if (info->acc_nthreads != NULL) {
                    // In Accelerate, there is a symbol called APPLE_NTHREADS, which appears to be a function we
                    // can call to get an integer saying the number of CPU threads. There is no documentation for this
                    // anywhere accessible online, but testing two different CPUs seem to suggest it is CPU cores.
                    //
                    // Doing this:
                    //     julia> @ccall AppleAccelerate.libacc.APPLE_NTHREADS()::Int
                    //
                    // The M2 Max returned 12, M4 Max returned 16, which is the total number of cores (both big and little)
                    // in each processor.
                    int nthreads = info->acc_nthreads();
                    max_threads = max(max_threads, nthreads);
                } else {
                    // This number is arbitrary because we have no idea how many threads are actually in use,
                    // but greater than 1 to mean multi-threaded.
                    max_threads = max(max_threads, 2);
                }
            } else {
                // Single-threaded
                max_threads = max(max_threads, 1);
            }
        }
    }
    return max_threads;
}



/*
 * Sets the given number of threads for all loaded libraries.
 */
LBT_DLLEXPORT void lbt_set_num_threads(int32_t nthreads) {
    const int num_libs = threading_cache_num_libs;
    for (int lib_idx=0; lib_idx < num_libs; ++lib_idx) {
        const lbt_threading_info_t * info = &threading_cache[lib_idx];
        for (int idx=0; idx < info->num_setters; ++idx) {
            info->setters[idx](nthreads);
        }

        // Special-case MKL, as we need to specifically ask for the BLAS & LAPACK domains
        if (info->mkl_set != NULL) {
            info->mkl_set(nthreads, MKL_DOMAIN_BLAS);
            info->mkl_set(nthreads, MKL_DOMAIN_LAPACK);
        }

        // Special case Apple Accelerate because we have to determine if we must set multi-threaded or single-threaded
        // This API only exists on macOS 15+.
        if (info->acc_set != NULL) {
            if (nthreads > 1) {
                info->acc_set(ACCELERATE_BLAS_THREADING_MULTI_THREADED);
            } else {
                info->acc_set(ACCELERATE_BLAS_THREADING_SINGLE_THREADED);
            }
        }
    }
}

#include "libblastrampoline_internal.h"

#define MAX_THREADING_NAMES     32

/*
 * We provide a flexible thread getter/setter interface here; by calling `lbt_set_num_threads()`
 * libblastrampoline will propagate the call through to its loaded libraries as long as the
 * library exposes a getter/setter method named in one of these arrays, and its signature
 * matches the common theme here.
 */
static char * getter_names[MAX_THREADING_NAMES] = {
    "openblas_get_num_threads",
    "mkl_get_max_threads",
    "bli_thread_set_num_threads",
    NULL
};

static char * setter_names[MAX_THREADING_NAMES] = {
    "openblas_set_num_threads",
    "mkl_set_num_threads",
    "bli_thread_get_num_threads",
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

    getter_names[idx] = (char *) malloc(strlen(getter));
    strcpy(getter_names[idx], getter);
    setter_names[idx] = (char *) malloc(strlen(setter));
    strcpy(setter_names[idx], setter);
}

/*
 * Returns the number of threads configured in all loaded libraries.
 * In the event of a mismatch, returns the largest value.
 */
LBT_DLLEXPORT int32_t lbt_get_num_threads() {
    int32_t max_threads = 0;

    const lbt_config_t * config = lbt_get_config();
    for (int lib_idx=0; config->loaded_libs[lib_idx] != NULL; ++lib_idx) {
        lbt_library_info_t * lib = config->loaded_libs[lib_idx];
        for (int symbol_idx=0; getter_names[symbol_idx] != NULL; ++symbol_idx) {
            char symbol_name[MAX_SYMBOL_LEN];
            sprintf(symbol_name, "%s%s", getter_names[symbol_idx], lib->suffix);
            int (*fptr)() = lookup_symbol(lib->handle, symbol_name);
            if (fptr != NULL) {
                int new_threads = fptr();
                max_threads = max_threads > new_threads ? max_threads : new_threads;
            }
        }
    }
    return max_threads;
}



/*
 * Sets the given number of threads for all loaded libraries.
 */
LBT_DLLEXPORT int32_t lbt_set_num_threads(int32_t nthreads) {
    const lbt_config_t * config = lbt_get_config();
    for (int lib_idx=0; config->loaded_libs[lib_idx] != NULL; ++lib_idx) {
        lbt_library_info_t * lib = config->loaded_libs[lib_idx];
        for (int symbol_idx=0; setter_names[symbol_idx] != NULL; ++symbol_idx) {
            char symbol_name[MAX_SYMBOL_LEN];
            sprintf(symbol_name, "%s%s", setter_names[symbol_idx], lib->suffix);
            void (*fptr)(int) = lookup_symbol(lib->handle, symbol_name);
            if (fptr != NULL) {
                fptr(nthreads);
            }
        }
    }
}

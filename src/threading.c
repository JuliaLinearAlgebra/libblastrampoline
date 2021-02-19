#include "libblastrampoline_internal.h"

static const char * getter_names[] = {
    "openblas_get_num_threads",
    "openblas_get_num_threads_64",
    "mkl_get_max_threads",
    NULL
};

/*
 * Returns the number of threads configured in all loaded libraries.
 * In the event of a mismatch, returns the largest value.
 */
JL_DLLEXPORT int32_t lbt_get_num_threads() {
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


static const char * setter_names[] = {
    "openblas_set_num_threads",
    "openblas_set_num_threads_64",
    "mkl_set_num_threads",
    NULL
};

/*
 * Sets the given number of threads for all loaded libraries.
 */
JL_DLLEXPORT int32_t lbt_set_num_threads(int32_t nthreads) {
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
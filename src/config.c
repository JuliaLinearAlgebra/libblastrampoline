#include "libblastrampoline_internal.h"
#include "exported_funcs.inc"

lbt_config_t lbt_config;

void init_config() {
    lbt_config.loaded_libs = (lbt_library_info_t **)malloc(sizeof(lbt_library_info_t *)*(MAX_TRACKED_LIBS + 1));
    memset(lbt_config.loaded_libs, 0, sizeof(lbt_library_info_t*)*(MAX_TRACKED_LIBS + 1));
    lbt_config.build_flags = 0x00000000;
}

LBT_DLLEXPORT const lbt_config_t * lbt_get_config() {
    // Set build flags (e.g. what we are capable of)
    lbt_config.build_flags = 0x00000000;

#if defined(LBT_DEEPBINDLESS)
    lbt_config.build_flags |= LBT_BUILDFLAGS_DEEPBINDLESS;
#endif

#if defined(F2C_AUTODETECTION)
    lbt_config.build_flags |= LBT_BUILDFLAGS_F2C_CAPABLE;
#endif

#if defined(CBLAS_DIVERGENCE_AUTODETECTION)
    lbt_config.build_flags |= LBT_BUILDFLAGS_CBLAS_DIVERGENCE;
#endif

#if defined(COMPLEX_RETSTYLE_AUTODETECTION)
    lbt_config.build_flags |= LBT_BUILDFLAGS_COMPLEX_RETSTYLE;
#endif

#if defined(SYMBOL_TRIMMING)
    lbt_config.build_flags |= LBT_BUILDFLAGS_SYMBOL_TRIMMING;
#endif

    lbt_config.exported_symbols = (const char **)&exported_func_names[0];
    lbt_config.num_exported_symbols = NUM_EXPORTED_FUNCS;

    return &lbt_config;
}

void clear_loaded_libraries() {
    for (int idx=0; idx<MAX_TRACKED_LIBS; ++idx) {
        if (lbt_config.loaded_libs[idx] != NULL) {
            free(lbt_config.loaded_libs[idx]->libname);
            free(lbt_config.loaded_libs[idx]->active_forwards);
            //close_library(lbt_config.loaded_libs[idx]->handle);
            free(lbt_config.loaded_libs[idx]);
            lbt_config.loaded_libs[idx] = NULL;
        }
    }
}

void clear_forwarding_mark(int32_t symbol_idx, int32_t interface) {
    for (int idx=0; idx<MAX_TRACKED_LIBS; ++idx) {
        if (lbt_config.loaded_libs[idx] == NULL) {
            return;
        }
        if (lbt_config.loaded_libs[idx]->interface != interface) {
            continue;
        }

        BITFIELD_CLEAR(lbt_config.loaded_libs[idx]->active_forwards, symbol_idx);
    }
}

void clear_other_forwards(int skip_idx, uint8_t * forwards, int32_t interface) {
    for (int idx=0; idx<MAX_TRACKED_LIBS; ++idx) {
        if (lbt_config.loaded_libs[idx] == NULL) {
            return;
        }
        // Skip ourselves and things that don't match our interface
        if (idx == skip_idx || lbt_config.loaded_libs[idx]->interface != interface) {
            continue;
        }

        // Flip off anything in this library that is flipped on in the passed-in forwards
        for (uint32_t chunk_idx=0; chunk_idx < (NUM_EXPORTED_FUNCS/8)+1; ++chunk_idx) {
            lbt_config.loaded_libs[idx]->active_forwards[chunk_idx] &= (forwards[chunk_idx] ^ 0xff);
        }
    }
}

void record_library_load(const char * libname, void * handle, const char * suffix, uint8_t * forwards,
                         int32_t interface, int32_t complex_retstyle, int32_t f2c, int32_t cblas) {
    // Scan for the an empty slot, and also check to see if this library has been loaded before.
    int free_idx = -1;
    for (int idx=0; idx<MAX_TRACKED_LIBS; ++idx) {
        if (lbt_config.loaded_libs[idx] == NULL) {
            free_idx = idx;
            break;
        }
        // If this library has been loaded before, and the interface matches, all we do is copy the `forwards` over
        // We check interface matching because it is possible for a single library to contain both LP64 and ILP64
        // symbols, and we want to treat both of those as separate.
        if (lbt_config.loaded_libs[idx]->handle == handle && lbt_config.loaded_libs[idx]->interface == interface) {
            memcpy(lbt_config.loaded_libs[idx]->active_forwards, forwards, sizeof(uint8_t)*(NUM_EXPORTED_FUNCS/8 + 1));
            clear_other_forwards(idx, forwards, interface);
            return;
        }
    }
    if (free_idx == -1) {
        // Uh-oh.  Someone has tried to load more than MAX_TRACKED_LIBS at a time!
        return;
    }

    lbt_library_info_t * new_libinfo = (lbt_library_info_t *) malloc(sizeof(lbt_library_info_t));

    size_t namelen = strlen(libname) + 1;
    new_libinfo->libname = (char *) malloc(namelen);
    memcpy(new_libinfo->libname, libname, namelen);
    new_libinfo->handle = handle;
    new_libinfo->suffix = suffix;
    new_libinfo->active_forwards = (uint8_t *)malloc(sizeof(uint8_t)*(NUM_EXPORTED_FUNCS/8 + 1));
    memcpy(new_libinfo->active_forwards, forwards, sizeof(uint8_t)*(NUM_EXPORTED_FUNCS/8 + 1));
    new_libinfo->interface = interface;
    new_libinfo->complex_retstyle = complex_retstyle;
    new_libinfo->f2c = f2c;
    new_libinfo->cblas = cblas;

    lbt_config.loaded_libs[free_idx] = new_libinfo;

    // Next, we go through and un-set all other loaded libraries of the same interface's `forwards`:
    clear_other_forwards(free_idx, forwards, interface);
}

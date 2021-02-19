#include "libblastrampoline_internal.h"

lbt_config_t lbt_config;

void init_config() {
    lbt_config.loaded_libs = (lbt_library_info_t **)malloc(sizeof(lbt_library_info_t *)*(MAX_TRACKED_LIBS + 1));
    memset(lbt_config.loaded_libs, 0, sizeof(lbt_library_info_t*)*(MAX_TRACKED_LIBS + 1));
    lbt_config.build_flags = 0x00000000;
}

JL_DLLEXPORT const lbt_config_t * lbt_get_config() {
    // Set build flags (e.g. what we are capable of)
    lbt_config.build_flags = 0x00000000;

#if defined(LBT_DEEPBINDLESS)
    lbt_config.build_flags |= LBT_BUILDFLAGS_DEEPBINDLESS;
#endif

#if defined(F2C_AUTODETECTION)
    lbt_config.build_flags |= LBT_BUILDFLAGS_F2C_CAPABLE;
#endif

    return &lbt_config;
}

void clear_loaded_libraries() {
    for (int idx=0; idx<MAX_TRACKED_LIBS; ++idx) {
        if (lbt_config.loaded_libs[idx] != NULL) {
            free(lbt_config.loaded_libs[idx]->libname);
            dlclose(lbt_config.loaded_libs[idx]->handle);
            free(lbt_config.loaded_libs[idx]);
            lbt_config.loaded_libs[idx] = NULL;
        }
    }
}

void record_library_load(const char * libname, void * handle, const char * suffix, int interface, int f2c) {
    // Scan for the an empty slot, and also check to see if this library has been loaded before.
    int free_idx = -1;
    for (int idx=0; idx<MAX_TRACKED_LIBS; ++idx) {
        if (lbt_config.loaded_libs[idx] == NULL) {
            free_idx = idx;
            break;
        }
        // If this library has been loaded before, just early-exit.
        if (lbt_config.loaded_libs[idx]->handle == handle) {
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
    new_libinfo->interface = interface;
    new_libinfo->f2c = f2c;

    lbt_config.loaded_libs[free_idx] = new_libinfo;
}
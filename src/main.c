#include "libblastrampoline_internal.h"

// dlopen() polymorph
static void * load_library(const char * path) {
    void * handle = NULL;
#if defined(_OS_WINDOWS_)
    wchar_t wpath[2*PATH_MAX + 1] = {0};
    if (!utf8_to_wchar(path, wpath, 2*PATH_MAX)) {
        jl_loader_print_stderr3("ERROR: Unable to convert path ", path, " to wide string!\n");
        exit(1);
    }
    handle = (void *)LoadLibraryExW(wpath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
#else
    handle = dlopen(path, RTLD_NOW | RTLD_GLOBAL);
#endif

    if (handle == NULL) {
        fprintf(stderr, "ERROR: Unable to load dependent library %s\n", path);
#if defined(_OS_WINDOWS_)
        LPWSTR wmsg = TEXT("");
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                       FORMAT_MESSAGE_FROM_SYSTEM |
                       FORMAT_MESSAGE_IGNORE_INSERTS |
                       FORMAT_MESSAGE_MAX_WIDTH_MASK,
                       NULL, GetLastError(),
                       MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                       (LPWSTR)&wmsg, 0, NULL);
        char err[256] = {0};
        wchar_to_utf8(wmsg, err, 255);        
#else
        const char * err = dlerror();
#endif
        fprintf(stderr, "Message: %s\n", err);
        exit(1);
    }
    return handle;
}

// dlsym() polymorph
static void * lookup_symbol(const void * lib_handle, const char * symbol_name) {
#ifdef _OS_WINDOWS_
    return GetProcAddress((HMODULE) lib_handle, symbol_name);
#else
    return dlsym((void *)lib_handle, symbol_name);
#endif
}

JL_DLLEXPORT int set_blas_funcs(const char * libblas_name) {
    printf("Generating forwards to %s\n", libblas_name);

    // Load the BLAS lib
    void * libblas = load_library(libblas_name);
    if (libblas == NULL) {
        fprintf(stderr, "Unable to load libblas %s\n", libblas_name);
        return 0;
    }

    // Once we have libblas loaded, re-export its symbols:
    int nforwards = 0;
    int symbol_idx = 0;
    for (symbol_idx=0; jl_exported_func_names[symbol_idx] != NULL; ++symbol_idx) {
      void *addr = lookup_symbol(libblas, jl_exported_func_names[symbol_idx]);
      (*jl_exported_func_addrs[symbol_idx]) = addr;
      //printf("%d: %s: %x\n", symbol_idx, jl_exported_func_names[symbol_idx], addr);
      if (addr != NULL) ++nforwards;
    }

    //printf("Reserved space for %d symbols; processed %d symbols; forwarded %d symbols\n", sizeof(jl_exported_func_names)/sizeof(char *), symbol_idx, nforwards);
    return 1;
}

__attribute__((constructor)) void init(void) {
    const char * libblas_name = NULL;
    libblas_name = getenv("LIBBLAS_NAME");
    if (libblas_name != NULL) {
        set_blas_funcs(libblas_name);
    }
}

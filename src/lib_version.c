#include "libblastrampoline_internal.h"

/* We need to ask MKL for information about itself to get better information for the config.
 *
 * These are from mkl_types.h
 */
typedef struct {
    int    MajorVersion;
    int    MinorVersion;
    int    UpdateVersion;
    int    PatchVersion;
    char * ProductStatus;
    char * Build;
    char * Processor;
    char * Platform;
} MKLVersion;


// Every library implements their version string handling differently, so this is a ratsnest
// of conditions for the various libraries to try and get information that is useful to us...
char* lbt_get_library_info(lbt_library_info_t* library)
{
    // Keep the string as a static lifetime so that it never gets deleted
    int len_info = 255;
    static char info[255];

    // Remove stale information from info
    info[0] = '\0';

    // OpenBLAS, config will have same suffix as the other functions
    char symbol_name[MAX_SYMBOL_LEN];
    build_symbol_name(symbol_name, "openblas_get_config", library->suffix);
    char* (*fptr_openblas)() = lookup_symbol(library->handle, symbol_name);
    if (fptr_openblas != NULL) {
        char* tmp_info = fptr_openblas();
        strcpy(info, tmp_info);
        return info;
    }

    // MKL
    char* (*fptr_mkl)(char*, int) = lookup_symbol(library->handle, "mkl_get_version_string");
    if (fptr_mkl != NULL) {
        fptr_mkl(info, len_info);
        return info;
    }

    // NVPL
    int (*fptr_nvpl)() = lookup_symbol(library->handle, "nvpl_blas_get_version");
    if (fptr_nvpl != NULL) {
        int version = fptr_nvpl();

        // The version int is of the form:
        // NVPL_BLAS_VERSION_MAJOR * 10000 + NVPL_BLAS_VERSION_MINOR * 100 + NVPL_BLAS_VERSION_PATCH
        int major = version / 10000;
        int minor = (version - (major*10000)) / 100;
        int patch = (version - (major*10000) - (minor*100));

        snprintf(info, len_info, "NVPL %d.%d.%d", major, minor, patch);
        return info;
    }

    // ARMPL
    int (*fptr_armpl)(int*, int*, int*, char**) = lookup_symbol(library->handle, "armplversion");
    if (fptr_armpl != NULL) {
        int major = 0, minor = 0, patch = 0;
        char* tag = NULL;
        fptr_armpl(&major, &minor, &patch, &tag);

        snprintf(info, len_info, "ARMPL %d.%d.%d.%s", major, minor, patch, tag);
        return info;
    }

    // AOCL and BLIS share the same methods
    char * (*fptr_blis_ver)() = lookup_symbol(library->handle, "bli_info_get_version_str");
    if (fptr_blis_ver != NULL) {
        int aocl_detected = 0;
        int int_size = 0;
        char* config = NULL;

        // Raw version string
        char* ver_str = fptr_blis_ver();

        // Integer size
        int (*fptr_blis_int)() = lookup_symbol(library->handle, "bli_info_get_blas_int_type_size");
        if (fptr_blis_int != NULL) {
            int_size = fptr_blis_int();
        }

        // Current architecture
        int (*fptr_blis_arch)() = lookup_symbol(library->handle, "bli_arch_query_id");
        char* (*fptr_blis_arch_str)(int) = lookup_symbol(library->handle, "bli_arch_string");
        if (fptr_blis_arch != NULL && fptr_blis_arch_str != NULL) {
            int arch = fptr_blis_arch();
            config = fptr_blis_arch_str(arch);
        }

        // Determine if the library is AOCL or not - it uses the same exact symbols as BLIS, but it also exposes some new symbols
        // that are AOCL-only that we can use to check if it is AOCL.
        int (*fptr_aocl)() = lookup_symbol(library->handle, "bli_aocl_enable_instruction_query");
        if (fptr_aocl != NULL) {
            // AOCL
            aocl_detected = 1;
        }

        snprintf(info, len_info, "%s %s, %d-bit integer, %s",
                 aocl_detected == 1 ? "AOCL" : "BLIS",
                 ver_str,
                 int_size,
                 config);

        return info;
    }

    // FlexiBLAS
    void (*fptr_flexi_ver)(int*, int*, int*) = lookup_symbol(library->handle, "flexiblas_get_version");
    if (fptr_flexi_ver != NULL) {
        int major = 0, minor = 0, patch = 0;
        char backend[128];

        fptr_flexi_ver(&major, &minor, &patch);

        int(*fptr_flexi_backend)(char*, int) = lookup_symbol(library->handle, "flexiblas_current_backend");
        if (fptr_flexi_backend != NULL) {
            fptr_flexi_backend(backend, 128);
        }

        snprintf(info, len_info, "FlexiBLAS %d.%d.%d, backend: %s", major, minor, patch, backend);
        return info;
    }


    return "Unknown library";
}

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Load in platform-detection macros
#include "platform.h"

#ifdef _OS_LINUX_
#include <linux/limits.h>
#include <libgen.h>
#include <dlfcn.h>
#endif

#ifdef _OS_WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef _OS_DARWIN_
#include <mach-o/dyld.h>
#include <sys/syslimits.h>
#include <dlfcn.h>
#endif

#ifdef _OS_FREEBSD_
#include <stddef.h>
#include <sys/sysctl.h>
#include <dlfcn.h>
#include <limits.h>
#endif

// This is the maximum length of a symbol that we'll allow
#define MAX_SYMBOL_LEN 64

// Functions in `win_utils.c`
int wchar_to_utf8(const wchar_t * wstr, char *str, size_t maxlen);
int utf8_to_wchar(const char * str, wchar_t * wstr, size_t maxlen);

// Functions in `dl_utils.c`
void * load_library(const char * path);
void * lookup_symbol(const void * lib_handle, const char * symbol_name);

// Functions in `autodetection.c`
const char * autodetect_symbol_suffix(void * handle);
int autodetect_blas_interface(void * isamax_addr);
int autodetect_lapack_interface(void * dpotrf_addr);
int autodetect_interface(void * handle, const char * suffix);

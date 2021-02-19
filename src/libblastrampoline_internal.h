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

#if !defined(RTLD_DEEPBIND) && (defined(_OS_LINUX_) || defined(_OS_FREEBSD_))
#define LBT_DEEPBINDLESS
#endif

// This is the maximum length of a symbol that we'll allow
#define MAX_SYMBOL_LEN 64

// Data defined in `libblastrampoline_trampdata.h
extern const char *const exported_func_names[];
extern const void ** exported_func32_addrs[];
extern const void ** exported_func64_addrs[];

// The config type you get back from lbt_get_config()
#define MAX_TRACKED_LIBS        31
typedef struct {
    char * libname;
    void * handle;
    const char * suffix;
    int32_t interface;
    int32_t f2c;
} lbt_library_info_t;

#define LBT_INTERFACE_LP64              32
#define LBT_INTERFACE_ILP64             64
#define LBT_INTERFACE_UNKNOWN           -1

#define LBT_F2C_PLAIN                   0
#define LBT_F2C_REQUIRED                1
#define LBT_F2C_UNKNOWN                 -1

typedef struct {
    lbt_library_info_t ** loaded_libs;
    uint32_t build_flags;
} lbt_config_t;

// The various "build_flags" that LBT can report back to the client
#define LBT_BUILDFLAGS_DEEPBINDLESS     0x01
#define LBT_BUILDFLAGS_F2C_CAPABLE      0x02

// Functions in `config.c`
void init_config();
void clear_loaded_libraries();
JL_DLLEXPORT const lbt_config_t * lbt_get_config();
void record_library_load(const char * libname, void * handle, const char * suffix, int interface, int f2c);

// Functions in `win_utils.c`
int wchar_to_utf8(const wchar_t * wstr, char *str, size_t maxlen);
int utf8_to_wchar(const char * str, wchar_t * wstr, size_t maxlen);

// Functions in `dl_utils.c`
void * load_library(const char * path);
void * lookup_symbol(const void * lib_handle, const char * symbol_name);
void close_library(void * handle);

// Functions in `autodetection.c`
const char * autodetect_symbol_suffix(void * handle);
int32_t autodetect_blas_interface(void * isamax_addr);
int32_t autodetect_lapack_interface(void * dpotrf_addr);
int32_t autodetect_interface(void * handle, const char * suffix);

#ifdef F2C_AUTODETECTION
int autodetect_f2c(void * handle, const char * suffix);
#endif

// Functions in surrogates.c
void push_fake_lsame();
void pop_fake_lsame();
int fake_lsame(char * ca, char * cb);

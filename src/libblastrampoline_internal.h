#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

// Load in our publicly-defined functions/types
#include "libblastrampoline.h"

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

#if defined(_OS_FREEBSD_) || defined(_OS_OPENBSD_)
#include <stddef.h>
#include <sys/sysctl.h>
#include <dlfcn.h>
#include <limits.h>
#endif

#ifdef _OS_HAIKU_
#include <posix/limits.h>
#include <posix/libgen.h>
#include <posix/dlfcn.h>
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

// Functions in `libblastrampoline.c`
int32_t find_symbol_idx(const char * name);

// Functions in `config.c`
void init_config();
void clear_loaded_libraries();
void clear_forwarding_mark(int32_t symbol_idx, int32_t interface);
void record_library_load(const char * libname, void * handle, const char * suffix, uint8_t * forwards,
                         int32_t interface, int32_t complex_retstyle, int32_t f2c, int32_t cblas);

// Functions in `win_utils.c`
#ifdef _OS_WINDOWS_
int wchar_to_utf8(const wchar_t * wstr, char *str, size_t maxlen);
int utf8_to_wchar(const char * str, wchar_t * wstr, size_t maxlen);
#endif

// Functions in `dl_utils.c`
void * load_library(const char * path);
void * lookup_symbol(const void * lib_handle, const char * symbol_name);
void * lookup_self_symbol(const char * symbol_name);
const char * lookup_self_path();
void close_library(void * handle);

// Functions in `env_utils.c`
uint8_t env_lowercase_match(const char * env_name, const char * value);
uint8_t env_lowercase_match_any(const char * env_name, uint32_t num_values, ...);
uint8_t env_match_bool(const char * env_name, uint8_t default_value);

// Functions in `autodetection.c`
void build_symbol_name(char * out, const char *symbol_name, const char *suffix);
const char * autodetect_symbol_suffix(void * handle, const char * suffix_hint);
int32_t autodetect_blas_interface(void * isamax_addr);
int32_t autodetect_lapack_interface(void * dpotrf_addr);
int32_t autodetect_interface(void * handle, const char * suffix);
#ifdef COMPLEX_RETSTYLE_AUTODETECTION
int32_t autodetect_complex_return_style(void * handle, const char * suffix);
#endif

#ifdef F2C_AUTODETECTION
int32_t autodetect_f2c(void * handle, const char * suffix);
#endif
#ifdef CBLAS_DIVERGENCE_AUTODETECTION
int32_t autodetect_cblas_divergence(void * handle, const char * suffix);
#endif

// Functions in deepbindless_surrogates.c
uint8_t push_fake_lsame();
uint8_t pop_fake_lsame();
int fake_lsame(char * ca, char * cb);
extern uint8_t use_deepbind;

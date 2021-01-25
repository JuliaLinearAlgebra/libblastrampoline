#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>

// Load in platform-detection macros
#include "platform.h"

#ifdef _OS_WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef _OS_DARWIN_
#include <mach-o/dyld.h>
#endif

#ifdef _OS_FREEBSD_
#include <stddef.h>
#include <sys/sysctl.h>
#endif

// Load in our exported function names, they'll be stored in a macro called
// JL_EXPORTED_FUNCS(XX), and we'll do item-by-item processing by defining XX.
#include "jl_exported_funcs.inc"

// Define holder locations for function addresses as `const void * $(name)_addr`
#define XX(name)    JL_HIDDEN const void * name##_addr;
JL_EXPORTED_FUNCS(XX)
#undef XX

// Generate lists of function names and addresses
#define XX(name)    #name,
static const char *const jl_exported_func_names[] = {
    JL_EXPORTED_FUNCS(XX)
    NULL
};
#undef XX

#define XX(name)    &name##_addr,
static const void ** jl_exported_func_addrs[] = {
    JL_EXPORTED_FUNCS(XX)
    NULL
};
#undef XX

#include "preprocessor_hacks.h"
#include "../exported_funcs.inc"

// On macOS, we need to prepend underscores on symbols
#if defined(__APPLE__) && defined(__MACH__)
#define CNAME(x)    _##x
#define PAGE(x)     x##@PAGE
#define PAGEOFF(x)  x##@PAGEOFF
#define SEP         %%
#else
#define CNAME(x)    x
#define PAGE(x)     x
#define PAGEOFF(x)  #:lo12:##x
#define SEP         ;
#endif

#define XX(name) \
.global CNAME(name) SEP \
.cfi_startproc SEP \
.p2align    2 SEP \
CNAME(name)##: SEP \
    adrp x16, PAGE(CNAMEADDR(name)) SEP \
    ldr x16, [x16, PAGEOFF(CNAMEADDR(name))] SEP \
    br x16 SEP \
.cfi_endproc SEP \


// Once for the 32-bit versions
EXPORTED_FUNCS(XX)
#undef CNAME

// Once for the 64-bit versions
#if defined(__APPLE__) && defined(__MACH__)
#define CNAME(x)    _##x##64_
#else
#define CNAME(x)    x##64_
#endif

EXPORTED_FUNCS(XX)
#undef CNAME
#undef XX
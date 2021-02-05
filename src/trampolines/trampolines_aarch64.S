#include "preprocessor_hacks.h"
#include "../exported_funcs.inc"

// On macOS, we need to prepend underscores on symbols
#if defined(__APPLE__) && defined(__MACH__)
#define PAGE(x)     x##@PAGE
#define PAGEOFF(x)  x##@PAGEOFF
#define SEP         %%
#else
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

// Generate both `dgemm_` and `dgemm_64_`
#include "ilp64_doubling.h"
#undef XX
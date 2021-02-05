#include "preprocessor_hacks.h"
#include "../exported_funcs.inc"

#define XX(name) \
DEBUGINFO(name); \
.global CNAME(name); \
.cfi_startproc; \
SEH_START1(name); \
CNAME(name)##:; \
SEH_START2(); \
    CET_START(); \
    mov CNAMEADDR(name)(%rip),%r11; \
    jmpq *%r11; \
    ud2; \
SEH_END(); \
.cfi_endproc; \
EXPORT(name); \

// Generate both `dgemm_` and `dgemm_64_`
#include "ilp64_doubling.h"
#undef XX
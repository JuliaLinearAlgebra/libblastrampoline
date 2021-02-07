#include "common.h"
#include "../exported_funcs.inc"

#define XX(name) \
DEBUGINFO(name); \
.global MANGLE(UNDERSCORE(name)); \
.cfi_startproc; \
SEH_START1(name); \
MANGLE(UNDERSCORE(name))##:; \
SEH_START2(); \
    CET_START(); \
    mov NAMEADDR(name)(%rip),%r11; \
    jmpq *%r11; \
    ud2; \
SEH_END(); \
.cfi_endproc; \
EXPORT(name); \

// Generate both `dgemm_` and `dgemm_64_`
#include "ilp64_doubling.h"
#undef XX

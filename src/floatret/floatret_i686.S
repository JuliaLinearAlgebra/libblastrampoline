#include "common.h"
#include "../exported_funcs.inc"

#define XX(name, index) \
DEBUGINFO(name); \
.global MANGLE(UNDERSCORE(name)); \
.cfi_startproc; \
MANGLE(UNDERSCORE(name))##:; \
    CET_START(); \
    call *(NAMEADDR(name)); \
    fstp %xmm0; \
    cvtsd2ss %xmm0, %xmm0; \
    fld %xmm0; \
    ret; \
    ud2; \
.cfi_endproc; \
EXPORT(name); \

// Generate both `dgemm_` and `dgemm_64_`
#include "ilp64_doubling.h"
#undef XX

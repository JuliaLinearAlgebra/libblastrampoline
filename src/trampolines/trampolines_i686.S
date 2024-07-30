#include "common.h"
#include "../exported_funcs.inc"

#define XX(name, idx) \
DEBUGINFO(name); \
.global MANGLE(UNDERSCORE(name)); \
.cfi_startproc; \
MANGLE(UNDERSCORE(name))##:; \
    CET_START(); \
    mov $SYMBOL_IDX(idx),%eax; \
    jmpl *(NAMEADDR(name)); \
    ud2; \
.cfi_endproc; \
EXPORT(name); \

// Generate both `dgemm_` and `dgemm_64_`
#include "ilp64_doubling.h"
#undef XX

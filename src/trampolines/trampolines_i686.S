#include "preprocessor_hacks.h"
#include "../exported_funcs.inc"

#define XX(name) \
DEBUGINFO(CNAME(name)); \
.global CNAME(name); \
.cfi_startproc; \
CNAME(name)##:; \
    CET_START(); \
    jmpl *(CNAMEADDR(name)); \
    ud2; \
.cfi_endproc; \
EXPORT(name); \

// Generate both `dgemm_` and `dgemm_64_`
#include "ilp64_doubling.h"
#undef XX
#include "preprocessor_hacks.h"
#include "../exported_funcs.inc"

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
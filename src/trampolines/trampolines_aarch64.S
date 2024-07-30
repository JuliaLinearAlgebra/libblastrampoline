#include "common.h"
#include "../exported_funcs.inc"

#define XX(name, idx) \
.global MANGLE(UNDERSCORE(name)) SEP \
.cfi_startproc SEP \
.p2align    2 SEP \
MANGLE(UNDERSCORE(name))##: SEP \
    mov x17, SYMBOL_IDX(idx) SEP \
    adrp x16, PAGE(NAMEADDR(name)) SEP \
    ldr x16, [x16, PAGEOFF(NAMEADDR(name))] SEP \
    br x16 SEP \
.cfi_endproc SEP \

// Generate both `dgemm_` and `dgemm_64_`
#include "ilp64_doubling.h"
#undef XX

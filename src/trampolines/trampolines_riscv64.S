#include "common.h"
#include "../exported_funcs.inc"

#define SEP ;

#define XX(name) \
.global MANGLE(UNDERSCORE(name)) SEP \
.cfi_startproc SEP \
.p2align    2 SEP \
 MANGLE(UNDERSCORE(name))##: SEP \
    auipc t3, %pcrel_hi(NAMEADDR(name)) SEP \
    ld t3, %pcrel_lo(MANGLE(UNDERSCORE(name)))(t3) SEP \
    jr t3 SEP \
.cfi_endproc SEP \

// Generate both `dgemm_` and `dgemm_64_`
#include "ilp64_doubling.h"
#undef XX

#include "common.h"
#include "../exported_funcs.inc"

#define SEP ;

#define XX(name, idx) \
.global MANGLE(UNDERSCORE(name)) SEP \
.cfi_startproc SEP \
.p2align    2 SEP \
 MANGLE(UNDERSCORE(name))##: SEP \
    li.d $t8, SYMBOL_IDX(idx) SEP \
    la.pcrel $t3, NAMEADDR(name) SEP \
    ld.d $t3, $t3, 0 SEP \
    jr $t3 SEP \
    amswap.w $zero, $zero, $zero SEP \
.cfi_endproc SEP \

// Generate both `dgemm_` and `dgemm_64_`
#include "ilp64_doubling.h"
#undef XX

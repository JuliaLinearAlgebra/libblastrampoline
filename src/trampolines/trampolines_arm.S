#include "common.h"
#include "../exported_funcs.inc"

#define XX(name) \
.global MANGLE(UNDERSCORE(name)); \
.cfi_startproc; \
MANGLE(UNDERSCORE(name))##:; \
    ldr ip, CONCAT(.L,NAMEADDR(name)); \
CONCAT(.L,MANGLE(UNDERSCORE(name))): ;\
    add ip, pc, ip; \
    ldr pc, [ip]; \
    .align 2; \
CONCAT(.L,NAMEADDR(name))##: ; \
    .word NAMEADDR(name)##-(CONCAT(.L,MANGLE(UNDERSCORE(name))) + 8); \
.cfi_endproc; \

// Generate both `dgemm_` and `dgemm_64_`
#include "ilp64_doubling.h"
#undef XX

#include "common.h"
#include "../exported_funcs.inc"

#define XX(name) \
.global CNAME(name); \
.cfi_startproc; \
CNAME(name)##:; \
    ldr ip, .L##CNAME(name); \
.L##CNAME(name): ;\
    add ip, pc, ip; \
    ldr pc, [ip]; \
    .align 2; \
.L##CNAMEADDR(name)##: ; \
    .word CNAMEADDR(name)##-(.L##name + 8); \
.cfi_endproc; \

// Generate both `dgemm_` and `dgemm_64_`
#include "ilp64_doubling.h"
#undef XX

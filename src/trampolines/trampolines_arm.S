#include "preprocessor_hacks.h"
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


// Once for the 32-bit versions
#define CNAME(x)    x)
EXPORTED_FUNCS(XX)
#undef CNAME

// Once for the 64-bit versions
#define CNAME(x)    x##64_
EXPORTED_FUNCS(XX)
#undef CNAME
#undef XX

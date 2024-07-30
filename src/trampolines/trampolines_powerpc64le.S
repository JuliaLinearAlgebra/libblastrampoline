#include "common.h"
#include "../exported_funcs.inc"

// Notes:
// bctr: branch to CTR without LR update (tail-call)
// localentry: On PPC functions have a localentry that assumes r2 contains
//             the TOC pointer, and a global entry point that sets r2.
// See 64-Bit ELF V2 ABI Specification: Power Architecture v1.4

#define XX(name, idx) \
.global MANGLE(UNDERSCORE(name)); \
.type MANGLE(UNDERSCORE(name))##, @function; \
.cfi_startproc; \
MANGLE(UNDERSCORE(name))##: ; \
    li 11,SYMBOL_IDX(idx); \
    addis 2, 12, .TOC.-MANGLE(UNDERSCORE(name))##@ha; \
    addi 2, 2, .TOC.-MANGLE(UNDERSCORE(name))##@l; \
    .localentry MANGLE(UNDERSCORE(name))##,.-MANGLE(UNDERSCORE(name))##; \
    addis 12,2,NAMEADDR(name)##@toc@ha; \
    ld 12,NAMEADDR(name)##@toc@l(12); \
    mtctr 12; \
    bctr; \
.cfi_endproc; \
.size MANGLE(UNDERSCORE(name))##,.-MANGLE(UNDERSCORE(name))##; \

// Generate both `dgemm_` and `dgemm_64_`
#include "ilp64_doubling.h"
#undef XX

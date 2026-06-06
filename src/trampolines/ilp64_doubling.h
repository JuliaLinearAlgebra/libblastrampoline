// Once for the 32-bit versions
#define MANGLE(x)  x
#define SYMBOL_IDX(idx)  (idx)
EXPORTED_FUNCS(XX)
#undef MANGLE
#undef SYMBOL_IDX

// Once for the 64-bit versions
#define MANGLE(x)  CONCAT(x,64_)
#define SYMBOL_IDX(idx)  (idx + NUM_EXPORTED_FUNCS)
EXPORTED_FUNCS(XX)
#undef MANGLE
#undef SYMBOL_IDX

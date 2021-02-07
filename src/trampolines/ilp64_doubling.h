// Once for the 32-bit versions
#define MANGLE(x)  x
EXPORTED_FUNCS(XX)
#undef MANGLE

// Once for the 64-bit versions
#define MANGLE(x)  CONCAT(x,64_)
EXPORTED_FUNCS(XX)
#undef MANGLE

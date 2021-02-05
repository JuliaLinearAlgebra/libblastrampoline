// Once for the 32-bit versions
#define CNAME(x)    UNDERSCORE(x)
EXPORTED_FUNCS(XX)
#undef CNAME

// Once for the 64-bit versions
#define CNAME(x)    CONCAT(UNDERSCORE(x),64_)
EXPORTED_FUNCS(XX)
#undef CNAME
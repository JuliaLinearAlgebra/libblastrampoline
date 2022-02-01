// These hold the "true" address that the f2c wrapper invokes in turn
#define XX(name, idx)    LBT_HIDDEN const void * f2c_##name##_addr;
#define XX_64(name, idx) LBT_HIDDEN const void * f2c_##name##64__addr;
FLOAT32_FUNCS(XX)
FLOAT32_FUNCS(XX_64)
COMPLEX64_FUNCS(XX)
COMPLEX64_FUNCS(XX_64)
COMPLEX128_FUNCS(XX)
COMPLEX128_FUNCS(XX_64)
#undef XX
#undef XX_64

// Generate indices mapping each f2c function to its upstream symbol
#define XX(name, index)    index,
const int f2c_func_idxs[] = {
    FLOAT32_FUNCS(XX)
    COMPLEX64_FUNCS(XX)
    COMPLEX128_FUNCS(XX)
    -1
};
#undef XX


// Forward-declare some functions
#define XX(name, index)     extern const void * f2c_##name ;
#define XX_64(name, index)  extern const void * f2c_##name##64_ ;
FLOAT32_FUNCS(XX)
FLOAT32_FUNCS(XX_64)
COMPLEX64_FUNCS(XX)
COMPLEX64_FUNCS(XX_64)
COMPLEX128_FUNCS(XX)
COMPLEX128_FUNCS(XX_64)
#undef XX
#undef XX_64

// Generate list of function addresses to tie names -> f2c adapters
// These point to the actual wrappers, we'll generate the lists of addresses next
#define XX(name, index)    &f2c_##name,
#define XX_64(name, index) &f2c_##name##64_,
const void ** f2c_func32_wrappers[] = {
    FLOAT32_FUNCS(XX)
    COMPLEX64_FUNCS(XX)
    COMPLEX128_FUNCS(XX)
    NULL
};
const void ** f2c_func64_wrappers[] = {
    FLOAT32_FUNCS(XX_64)
    COMPLEX64_FUNCS(XX_64)
    COMPLEX128_FUNCS(XX_64)
    NULL
};
#undef XX
#undef XX_64


#define XX(name, index)    &f2c_##name##_addr,
#define XX_64(name, index) &f2c_##name##64__addr,
const void ** f2c_func32_addrs[] = {
    FLOAT32_FUNCS(XX)
    COMPLEX64_FUNCS(XX)
    COMPLEX128_FUNCS(XX)
    NULL
};
const void ** f2c_func64_addrs[] = {
    FLOAT32_FUNCS(XX_64)
    COMPLEX64_FUNCS(XX_64)
    COMPLEX128_FUNCS(XX_64)
    NULL
};
#undef XX
#undef XX_64

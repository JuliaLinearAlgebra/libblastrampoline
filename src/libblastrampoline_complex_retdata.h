// These hold the "true" address that the complex return wrapper invokes in turn
#define XX(name, idx)    LBT_HIDDEN const void * cmplxret_##name##_addr;
#define XX_64(name, idx) LBT_HIDDEN const void * cmplxret_##name##64__addr;
COMPLEX64_FUNCS(XX)
COMPLEX64_FUNCS(XX_64)
COMPLEX128_FUNCS(XX)
COMPLEX128_FUNCS(XX_64)
#undef XX
#undef XX_64

// Build mapping from cmplxret-index to `_addr` instance
#define XX(name, index)    &cmplxret_##name##_addr,
#define XX_64(name, index) &cmplxret_##name##64__addr,
const void ** cmplxret_func32_addrs[] = {
    COMPLEX64_FUNCS(XX)
    COMPLEX128_FUNCS(XX)
    NULL
};
const void ** cmplxret_func64_addrs[] = {
    COMPLEX64_FUNCS(XX_64)
    COMPLEX128_FUNCS(XX_64)
    NULL
};
#undef XX
#undef XX_64


// Forward-declare some functions
#define XX(name, index)     extern const void * cmplxret_##name ;
#define XX_64(name, index)  extern const void * cmplxret_##name##64_ ;
COMPLEX64_FUNCS(XX)
COMPLEX128_FUNCS(XX)
COMPLEX64_FUNCS(XX_64)
COMPLEX128_FUNCS(XX_64)
#undef XX
#undef XX_64


// Next, create an array that that points to all of our wrapper code
// locations, allowing a cblas index -> function lookup
#define XX(name, index)    &cmplxret_##name,
#define XX_64(name, index) &cmplxret_##name##64_,
const void ** cmplxret32_func_wrappers[] = {
    COMPLEX64_FUNCS(XX)
    COMPLEX128_FUNCS(XX)
    NULL
};
const void ** cmplxret64_func_wrappers[] = {
    COMPLEX64_FUNCS(XX_64)
    COMPLEX128_FUNCS(XX_64)
    NULL
};
#undef XX
#undef XX_64

// Finally, an array that maps cblas index -> exported symbol index
#define XX(name, index)    index,
const int cmplxret_func_idxs[] = {
    COMPLEX64_FUNCS(XX)
    COMPLEX128_FUNCS(XX)
    -1
};
#undef XX
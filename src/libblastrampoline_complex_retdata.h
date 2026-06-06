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
const void ** cmplx64ret_func32_addrs[] = {
    COMPLEX64_FUNCS(XX)
    NULL
};
const void ** cmplx128ret_func32_addrs[] = {
    COMPLEX128_FUNCS(XX)
    NULL
};
const void ** cmplx64ret_func64_addrs[] = {
    COMPLEX64_FUNCS(XX_64)
    NULL
};
const void ** cmplx128ret_func64_addrs[] = {
    COMPLEX128_FUNCS(XX_64)
    NULL
};
#undef XX
#undef XX_64

const void *** cmplxret_func32_addrs[] = {
    cmplx64ret_func32_addrs,
    cmplx128ret_func32_addrs
};
const void *** cmplxret_func64_addrs[] = {
    cmplx64ret_func64_addrs,
    cmplx128ret_func64_addrs
};



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
const void ** cmplx64ret_func32_wrappers[] = {
    COMPLEX64_FUNCS(XX)
    NULL
};
const void ** cmplx128ret_func32_wrappers[] = {
    COMPLEX128_FUNCS(XX)
    NULL
};
const void ** cmplx64ret_func64_wrappers[] = {
    COMPLEX64_FUNCS(XX_64)
    NULL
};
const void ** cmplx128ret_func64_wrappers[] = {
    COMPLEX128_FUNCS(XX_64)
    NULL
};
#undef XX
#undef XX_64

const void *** cmplxret_func32_wrappers[] = {
    cmplx64ret_func32_wrappers,
    cmplx128ret_func32_wrappers
};
const void *** cmplxret_func64_wrappers[] = {
    cmplx64ret_func64_wrappers,
    cmplx128ret_func64_wrappers
};



// Finally, an array that maps cmplxret index -> exported symbol index
#define XX(name, index)    index,
const int cmplx64ret_func_idxs[] = {
    COMPLEX64_FUNCS(XX)
    -1
};
const int cmplx128ret_func_idxs[] = {
    COMPLEX128_FUNCS(XX)
    -1
};
#undef XX

const int * cmplxret_func_idxs[] = {
    cmplx64ret_func_idxs,
    cmplx128ret_func_idxs
};

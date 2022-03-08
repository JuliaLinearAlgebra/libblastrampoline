#define XX(name, index)     extern const void * lbt_##name ;
#define XX_64(name, index)  extern const void * lbt_##name##64_ ;
CBLAS_WORKAROUND_FUNCS(XX)
CBLAS_WORKAROUND_FUNCS(XX_64)
#undef XX
#undef XX_64


// Next, create an array that that points to all of our wrapper code
// locations, allowing a cblas index -> function lookup
#define XX(name, index)    &lbt_##name,
#define XX_64(name, index) &lbt_##name##64_,
const void ** cblas32_func_wrappers[] = {
    CBLAS_WORKAROUND_FUNCS(XX)
    NULL
};
const void ** cblas64_func_wrappers[] = {
    CBLAS_WORKAROUND_FUNCS(XX_64)
    NULL
};
#undef XX
#undef XX_64

// Finally, an array that maps cblas index -> exported symbol index
#define XX(name, index)    index,
const int cblas_func_idxs[] = {
    CBLAS_WORKAROUND_FUNCS(XX)
    -1
};
#undef XX

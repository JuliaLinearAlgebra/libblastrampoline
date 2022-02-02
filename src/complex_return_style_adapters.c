#include <complex.h>
#include "libblastrampoline_internal.h"

/*
 * Some libraries use an argument-passing convention for returning complex numbers.
 * We create wrappers to work around this behavior and provide a consistent ABI
 * across all libraries.  An example of this style of library is MKL, x-ref:
 *  - https://community.intel.com/t5/Intel-oneAPI-Math-Kernel-Library/ARPACK-with-MKL-crashes-when-calling-zdotc/td-p/1054316
 *  - https://scicomp.stackexchange.com/questions/5380/intel-mkl-difference-between-mkl-intel-lp64-and-mkl-gf-lp64
 */


// zdotc
extern void (*cmplxret_zdotc__addr)(double complex * z,
                                    const int32_t *,
                                    const double complex *, const int32_t *,
                                    const double complex *, const int32_t *);
double complex cmplxret_zdotc_(const int32_t * N,
                               const double complex *X, const int32_t * incX,
                               const double complex *Y, const int32_t * incY)
{
   double complex c;
   cmplxret_zdotc__addr(&c, N, X, incX, Y, incY);
   return c;
}

extern void (*cmplxret_zdotc_64__addr)(double complex * z,
                                       const int64_t *,
                                       const double complex *, const int64_t *,
                                       const double complex *, const int64_t *);
double complex cmplxret_zdotc_64_(const int64_t * N,
                                  const double complex *X, const int64_t * incX,
                                  const double complex *Y, const int64_t * incY)
{
   double complex c;
   cmplxret_zdotc_64__addr(&c, N, X, incX, Y, incY);
   return c;
}


// zdotu
extern void (*cmplxret_zdotu__addr)(double complex * z,
                                    const int32_t *,
                                    const double complex *, const int32_t *,
                                    const double complex *, const int32_t *);
double complex cmplxret_zdotu_(const int32_t * N,
                               const double complex *X, const int32_t * incX,
                               const double complex *Y, const int32_t * incY)
{
   double complex c;
   cmplxret_zdotu__addr(&c, N, X, incX, Y, incY);
   return c;
}

extern void (*cmplxret_zdotu_64__addr)(double complex * z,
                                       const int64_t *,
                                       const double complex *, const int64_t *,
                                       const double complex *, const int64_t *);
double complex cmplxret_zdotu_64_(const int64_t * N,
                                  const double complex *X, const int64_t * incX,
                                  const double complex *Y, const int64_t * incY)
{
   double complex c;
   cmplxret_zdotu_64__addr(&c, N, X, incX, Y, incY);
   return c;
}


// cdotc
extern void (*cmplxret_cdotc__addr)(float complex * z,
                                    const int32_t *,
                                    const float complex *, const int32_t *,
                                    const float complex *, const int32_t *);
float complex cmplxret_cdotc_(const int32_t * N,
                               const float complex *X, const int32_t * incX,
                               const float complex *Y, const int32_t * incY)
{
   float complex c;
   cmplxret_cdotc__addr(&c, N, X, incX, Y, incY);
   return c;
}

extern void (*cmplxret_cdotc_64__addr)(float complex * z,
                                       const int64_t *,
                                       const float complex *, const int64_t *,
                                       const float complex *, const int64_t *);
float complex cmplxret_cdotc_64_(const int64_t * N,
                                  const float complex *X, const int64_t * incX,
                                  const float complex *Y, const int64_t * incY)
{
   float complex c;
   cmplxret_cdotc_64__addr(&c, N, X, incX, Y, incY);
   return c;
}


// cdotu
extern void (*cmplxret_cdotu__addr)(float complex * z,
                                    const int32_t *,
                                    const float complex *, const int32_t *,
                                    const float complex *, const int32_t *);
float complex cmplxret_cdotu_(const int32_t * N,
                               const float complex *X, const int32_t * incX,
                               const float complex *Y, const int32_t * incY)
{
   float complex c;
   cmplxret_cdotu__addr(&c, N, X, incX, Y, incY);
   return c;
}

extern void (*cmplxret_cdotu_64__addr)(float complex * z,
                                       const int64_t *,
                                       const float complex *, const int64_t *,
                                       const float complex *, const int64_t *);
float complex cmplxret_cdotu_64_(const int64_t * N,
                                  const float complex *X, const int64_t * incX,
                                  const float complex *Y, const int64_t * incY)
{
   float complex c;
   cmplxret_cdotu_64__addr(&c, N, X, incX, Y, incY);
   return c;
}


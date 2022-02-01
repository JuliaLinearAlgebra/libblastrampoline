#include <complex.h>
#include <stdint.h>
#include "libblastrampoline_internal.h"

/*
 * In MKL v2022.0, a new ILP64 interface was released, but it sadly lacked a few symbol names.
 * While waiting for a new release, the following intermediate 
 */

/*
double cblas_ddot_64(const int64_t N, const double *X, const int64_t incX, const double *Y, const int64_t incY)
{
   return ddot_64(&N, X, &incX, Y, &incY);
}*/

extern double complex (*mkl_cblas_zdotc_sub_64__addr)(const int64_t N,
                                                      const double complex *X, const int64_t incX,
                                                      const double complex *Y, const int64_t incY);
void mkl_cblas_zdotc_sub_64_(const int64_t N,
                             const double complex *X, const int64_t incX,
                             const double complex *Y, const int64_t incY,
                             double complex * z)
{
   *z = mkl_cblas_zdotc_sub_64__addr(N, X, incX, Y, incY);
}
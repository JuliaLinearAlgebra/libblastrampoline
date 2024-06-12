#include <complex.h>
#include "libblastrampoline_internal.h"

/*
 * Some libraries provide ILP64-suffixed FORTRAN symbols, but forgot the CBLAS ones.
 * To allow Julia to still use `cblas_{c,z}dot{c,u}_sub` when linking against the
 * explicitly ILP64-suffixed MKL libraries, we map the CBLAS forwards to the FORTRAN
 * symbols, where appropriate.  This effects MKL v2022.0, x-ref:
 *  - https://github.com/JuliaLinearAlgebra/libblastrampoline/issues/56
 */

extern double complex zdotc_(const int32_t *,
                             const double complex *, const int32_t *,
                             const double complex *, const int32_t *);
void lbt_cblas_zdotc_sub(const int32_t N,
                         const double complex *X, const int32_t incX,
                         const double complex *Y, const int32_t incY,
                         double complex * z)
{
   *z = zdotc_(&N, X, &incX, Y, &incY);
}

extern double complex zdotc_64_(const int64_t *,
                                const double complex *, const int64_t *,
                                const double complex *, const int64_t *);
void lbt_cblas_zdotc_sub64_(const int64_t N,
                            const double complex *X, const int64_t incX,
                            const double complex *Y, const int64_t incY,
                            double complex * z)
{
   *z = zdotc_64_(&N, X, &incX, Y, &incY);
}


extern double complex zdotu_(const int32_t *,
                             const double complex *, const int32_t *,
                             const double complex *, const int32_t *);
void lbt_cblas_zdotu_sub(const int32_t N,
                         const double complex *X, const int32_t incX,
                         const double complex *Y, const int32_t incY,
                         double complex * z)
{
   *z = zdotu_(&N, X, &incX, Y, &incY);
}

extern double complex zdotu_64_(const int64_t *,
                                const double complex *, const int64_t *,
                                const double complex *, const int64_t *);
void lbt_cblas_zdotu_sub64_(const int64_t N,
                            const double complex *X, const int64_t incX,
                            const double complex *Y, const int64_t incY,
                            double complex * z)
{
   *z = zdotu_64_(&N, X, &incX, Y, &incY);
}








extern float complex cdotc_(const int32_t *,
                             const float complex *, const int32_t *,
                             const float complex *, const int32_t *);
void lbt_cblas_cdotc_sub(const int32_t N,
                         const float complex *X, const int32_t incX,
                         const float complex *Y, const int32_t incY,
                         float complex * z)
{
   *z = cdotc_(&N, X, &incX, Y, &incY);
}

extern float complex cdotc_64_(const int64_t *,
                               const float complex *, const int64_t *,
                               const float complex *, const int64_t *);
void lbt_cblas_cdotc_sub64_(const int64_t N,
                            const float complex *X, const int64_t incX,
                            const float complex *Y, const int64_t incY,
                            float complex * z)
{
   *z = cdotc_64_(&N, X, &incX, Y, &incY);
}


extern float complex cdotu_(const int32_t *,
                             const float complex *, const int32_t *,
                             const float complex *, const int32_t *);
void lbt_cblas_cdotu_sub(const int32_t N,
                         const float complex *X, const int32_t incX,
                         const float complex *Y, const int32_t incY,
                         float complex * z)
{
   *z = cdotu_(&N, X, &incX, Y, &incY);
}

extern float complex cdotu_64_(const int64_t *,
                                const float complex *, const int64_t *,
                                const float complex *, const int64_t *);
void lbt_cblas_cdotu_sub64_(const int64_t N,
                            const float complex *X, const int64_t incX,
                            const float complex *Y, const int64_t incY,
                            float complex * z)
{
   *z = cdotu_64_(&N, X, &incX, Y, &incY);
}



extern float sdot_(const int32_t *,
                   const float *, const int32_t *,
                   const float *, const int32_t *);
float lbt_cblas_sdot(const int32_t N,
                     const float *X, const int32_t incX,
                     const float *Y, const int32_t incY)
{
   return sdot_(&N, X, &incX, Y, &incY);
}

extern float sdot_64_(const int64_t *,
                      const float  *, const int64_t *,
                      const float  *, const int64_t *);
float lbt_cblas_sdot64_(const int64_t N,
                        const float  *X, const int64_t incX,
                        const float  *Y, const int64_t incY)
{
   return sdot_64_(&N, X, &incX, Y, &incY);
}

extern double ddot_(const int32_t *,
                    const double *, const int32_t *,
                    const double *, const int32_t *);
double lbt_cblas_ddot(const int32_t N,
                      const double *X, const int32_t incX,
                      const double *Y, const int32_t incY)
{
   return ddot_(&N, X, &incX, Y, &incY);
}

extern double ddot_64_(const int64_t *,
                       const double  *, const int64_t *,
                       const double  *, const int64_t *);
double lbt_cblas_ddot64_(const int64_t N,
                         const double  *X, const int64_t incX,
                         const double  *Y, const int64_t incY)
{
   return ddot_64_(&N, X, &incX, Y, &incY);
}

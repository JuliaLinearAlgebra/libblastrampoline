#ifndef CBLAS_H
#define CBLAS_H

#include <stddef.h>
#include "openblas_config.h"

#ifdef __cplusplus
extern "C" {
	/* Assume C declarations for C++ */
#endif  /* __cplusplus */

/*Set the number of threads on runtime.*/
void openblas_set_num_threads64_(int num_threads);
void goto_set_num_threads64_(int num_threads);

/*Get the number of threads on runtime.*/
int openblas_get_num_threads64_(void);

/*Get the number of physical processors (cores).*/
int openblas_get_num_procs64_(void);

/*Get the build configure on runtime.*/
char* openblas_get_config64_(void);

/*Get the CPU corename on runtime.*/
char* openblas_get_corename64_(void);

#ifdef OPENBLAS_OS_LINUX
/* Sets thread affinity for OpenBLAS threads. `thread_idx` is in [0, openblas_get_num_threads64_()-1]. */
int openblas_setaffinity64_(int thread_idx, size_t cpusetsize, cpu_set_t* cpu_set);
#endif

/* Get the parallelization type which is used by OpenBLAS */
int openblas_get_parallel64_(void);
/* OpenBLAS is compiled for sequential use  */
#define OPENBLAS_SEQUENTIAL  0
/* OpenBLAS is compiled using normal threading model */
#define OPENBLAS_THREAD  1
/* OpenBLAS is compiled using OpenMP threading model */
#define OPENBLAS_OPENMP 2


/*
 * Since all of GotoBlas was written without const,
 * we disable it at build time.
 */
#ifndef OPENBLAS_CONST
# define OPENBLAS_CONST const
#endif


#define CBLAS_INDEX size_t

typedef enum CBLAS_ORDER     {CblasRowMajor=101, CblasColMajor=102} CBLAS_ORDER;
typedef enum CBLAS_TRANSPOSE {CblasNoTrans=111, CblasTrans=112, CblasConjTrans=113, CblasConjNoTrans=114} CBLAS_TRANSPOSE;
typedef enum CBLAS_UPLO      {CblasUpper=121, CblasLower=122} CBLAS_UPLO;
typedef enum CBLAS_DIAG      {CblasNonUnit=131, CblasUnit=132} CBLAS_DIAG;
typedef enum CBLAS_SIDE      {CblasLeft=141, CblasRight=142} CBLAS_SIDE;
typedef CBLAS_ORDER CBLAS_LAYOUT;
	
float  cblas_sdsdot64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST float alpha, OPENBLAS_CONST float *x, OPENBLAS_CONST blasint incx, OPENBLAS_CONST float *y, OPENBLAS_CONST blasint incy);
double cblas_dsdot64_ (OPENBLAS_CONST blasint n, OPENBLAS_CONST float *x, OPENBLAS_CONST blasint incx, OPENBLAS_CONST float *y, OPENBLAS_CONST blasint incy);
float  cblas_sdot64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST float  *x, OPENBLAS_CONST blasint incx, OPENBLAS_CONST float  *y, OPENBLAS_CONST blasint incy);
double cblas_ddot64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST double *x, OPENBLAS_CONST blasint incx, OPENBLAS_CONST double *y, OPENBLAS_CONST blasint incy);

openblas_complex_float   cblas_cdotu64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void  *x, OPENBLAS_CONST blasint incx, OPENBLAS_CONST void  *y, OPENBLAS_CONST blasint incy);
openblas_complex_float   cblas_cdotc64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void  *x, OPENBLAS_CONST blasint incx, OPENBLAS_CONST void  *y, OPENBLAS_CONST blasint incy);
openblas_complex_double  cblas_zdotu64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void *x, OPENBLAS_CONST blasint incx, OPENBLAS_CONST void *y, OPENBLAS_CONST blasint incy);
openblas_complex_double  cblas_zdotc64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void *x, OPENBLAS_CONST blasint incx, OPENBLAS_CONST void *y, OPENBLAS_CONST blasint incy);

void  cblas_cdotu_sub64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void  *x, OPENBLAS_CONST blasint incx, OPENBLAS_CONST void  *y, OPENBLAS_CONST blasint incy, void  *ret);
void  cblas_cdotc_sub64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void  *x, OPENBLAS_CONST blasint incx, OPENBLAS_CONST void  *y, OPENBLAS_CONST blasint incy, void  *ret);
void  cblas_zdotu_sub64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void *x, OPENBLAS_CONST blasint incx, OPENBLAS_CONST void *y, OPENBLAS_CONST blasint incy, void *ret);
void  cblas_zdotc_sub64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void *x, OPENBLAS_CONST blasint incx, OPENBLAS_CONST void *y, OPENBLAS_CONST blasint incy, void *ret);

float  cblas_sasum64_ (OPENBLAS_CONST blasint n, OPENBLAS_CONST float  *x, OPENBLAS_CONST blasint incx);
double cblas_dasum64_ (OPENBLAS_CONST blasint n, OPENBLAS_CONST double *x, OPENBLAS_CONST blasint incx);
float  cblas_scasum64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void  *x, OPENBLAS_CONST blasint incx);
double cblas_dzasum64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void *x, OPENBLAS_CONST blasint incx);

float  cblas_ssum64_ (OPENBLAS_CONST blasint n, OPENBLAS_CONST float  *x, OPENBLAS_CONST blasint incx);
double cblas_dsum64_ (OPENBLAS_CONST blasint n, OPENBLAS_CONST double *x, OPENBLAS_CONST blasint incx);
float  cblas_scsum64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void  *x, OPENBLAS_CONST blasint incx);
double cblas_dzsum64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void *x, OPENBLAS_CONST blasint incx);

float  cblas_snrm264_ (OPENBLAS_CONST blasint N, OPENBLAS_CONST float  *X, OPENBLAS_CONST blasint incX);
double cblas_dnrm264_ (OPENBLAS_CONST blasint N, OPENBLAS_CONST double *X, OPENBLAS_CONST blasint incX);
float  cblas_scnrm264_(OPENBLAS_CONST blasint N, OPENBLAS_CONST void  *X, OPENBLAS_CONST blasint incX);
double cblas_dznrm264_(OPENBLAS_CONST blasint N, OPENBLAS_CONST void *X, OPENBLAS_CONST blasint incX);

CBLAS_INDEX cblas_isamax64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST float  *x, OPENBLAS_CONST blasint incx);
CBLAS_INDEX cblas_idamax64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST double *x, OPENBLAS_CONST blasint incx);
CBLAS_INDEX cblas_icamax64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void  *x, OPENBLAS_CONST blasint incx);
CBLAS_INDEX cblas_izamax64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void *x, OPENBLAS_CONST blasint incx);

CBLAS_INDEX cblas_isamin64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST float  *x, OPENBLAS_CONST blasint incx);
CBLAS_INDEX cblas_idamin64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST double *x, OPENBLAS_CONST blasint incx);
CBLAS_INDEX cblas_icamin64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void  *x, OPENBLAS_CONST blasint incx);
CBLAS_INDEX cblas_izamin64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void *x, OPENBLAS_CONST blasint incx);

CBLAS_INDEX cblas_ismax64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST float  *x, OPENBLAS_CONST blasint incx);
CBLAS_INDEX cblas_idmax64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST double *x, OPENBLAS_CONST blasint incx);
CBLAS_INDEX cblas_icmax64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void  *x, OPENBLAS_CONST blasint incx);
CBLAS_INDEX cblas_izmax64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void *x, OPENBLAS_CONST blasint incx);

CBLAS_INDEX cblas_ismin64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST float  *x, OPENBLAS_CONST blasint incx);
CBLAS_INDEX cblas_idmin64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST double *x, OPENBLAS_CONST blasint incx);
CBLAS_INDEX cblas_icmin64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void  *x, OPENBLAS_CONST blasint incx);
CBLAS_INDEX cblas_izmin64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void *x, OPENBLAS_CONST blasint incx);

void cblas_saxpy64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST float alpha, OPENBLAS_CONST float *x, OPENBLAS_CONST blasint incx, float *y, OPENBLAS_CONST blasint incy);
void cblas_daxpy64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST double alpha, OPENBLAS_CONST double *x, OPENBLAS_CONST blasint incx, double *y, OPENBLAS_CONST blasint incy);
void cblas_caxpy64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *x, OPENBLAS_CONST blasint incx, void *y, OPENBLAS_CONST blasint incy);
void cblas_zaxpy64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *x, OPENBLAS_CONST blasint incx, void *y, OPENBLAS_CONST blasint incy);

void cblas_scopy64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST float *x, OPENBLAS_CONST blasint incx, float *y, OPENBLAS_CONST blasint incy);
void cblas_dcopy64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST double *x, OPENBLAS_CONST blasint incx, double *y, OPENBLAS_CONST blasint incy);
void cblas_ccopy64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void *x, OPENBLAS_CONST blasint incx, void *y, OPENBLAS_CONST blasint incy);
void cblas_zcopy64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void *x, OPENBLAS_CONST blasint incx, void *y, OPENBLAS_CONST blasint incy);

void cblas_sswap64_(OPENBLAS_CONST blasint n, float *x, OPENBLAS_CONST blasint incx, float *y, OPENBLAS_CONST blasint incy);
void cblas_dswap64_(OPENBLAS_CONST blasint n, double *x, OPENBLAS_CONST blasint incx, double *y, OPENBLAS_CONST blasint incy);
void cblas_cswap64_(OPENBLAS_CONST blasint n, void *x, OPENBLAS_CONST blasint incx, void *y, OPENBLAS_CONST blasint incy);
void cblas_zswap64_(OPENBLAS_CONST blasint n, void *x, OPENBLAS_CONST blasint incx, void *y, OPENBLAS_CONST blasint incy);

void cblas_srot64_(OPENBLAS_CONST blasint N, float *X, OPENBLAS_CONST blasint incX, float *Y, OPENBLAS_CONST blasint incY, OPENBLAS_CONST float c, OPENBLAS_CONST float s);
void cblas_drot64_(OPENBLAS_CONST blasint N, double *X, OPENBLAS_CONST blasint incX, double *Y, OPENBLAS_CONST blasint incY, OPENBLAS_CONST double c, OPENBLAS_CONST double  s);

void cblas_srotg64_(float *a, float *b, float *c, float *s);
void cblas_drotg64_(double *a, double *b, double *c, double *s);

void cblas_srotm64_(OPENBLAS_CONST blasint N, float *X, OPENBLAS_CONST blasint incX, float *Y, OPENBLAS_CONST blasint incY, OPENBLAS_CONST float *P);
void cblas_drotm64_(OPENBLAS_CONST blasint N, double *X, OPENBLAS_CONST blasint incX, double *Y, OPENBLAS_CONST blasint incY, OPENBLAS_CONST double *P);

void cblas_srotmg64_(float *d1, float *d2, float *b1, OPENBLAS_CONST float b2, float *P);
void cblas_drotmg64_(double *d1, double *d2, double *b1, OPENBLAS_CONST double b2, double *P);

void cblas_sscal64_(OPENBLAS_CONST blasint N, OPENBLAS_CONST float alpha, float *X, OPENBLAS_CONST blasint incX);
void cblas_dscal64_(OPENBLAS_CONST blasint N, OPENBLAS_CONST double alpha, double *X, OPENBLAS_CONST blasint incX);
void cblas_cscal64_(OPENBLAS_CONST blasint N, OPENBLAS_CONST void *alpha, void *X, OPENBLAS_CONST blasint incX);
void cblas_zscal64_(OPENBLAS_CONST blasint N, OPENBLAS_CONST void *alpha, void *X, OPENBLAS_CONST blasint incX);
void cblas_csscal64_(OPENBLAS_CONST blasint N, OPENBLAS_CONST float alpha, void *X, OPENBLAS_CONST blasint incX);
void cblas_zdscal64_(OPENBLAS_CONST blasint N, OPENBLAS_CONST double alpha, void *X, OPENBLAS_CONST blasint incX);

void cblas_sgemv64_(OPENBLAS_CONST enum CBLAS_ORDER order,  OPENBLAS_CONST enum CBLAS_TRANSPOSE trans,  OPENBLAS_CONST blasint m, OPENBLAS_CONST blasint n,
		 OPENBLAS_CONST float alpha, OPENBLAS_CONST float  *a, OPENBLAS_CONST blasint lda,  OPENBLAS_CONST float  *x, OPENBLAS_CONST blasint incx,  OPENBLAS_CONST float beta,  float  *y, OPENBLAS_CONST blasint incy);
void cblas_dgemv64_(OPENBLAS_CONST enum CBLAS_ORDER order,  OPENBLAS_CONST enum CBLAS_TRANSPOSE trans,  OPENBLAS_CONST blasint m, OPENBLAS_CONST blasint n,
		 OPENBLAS_CONST double alpha, OPENBLAS_CONST double  *a, OPENBLAS_CONST blasint lda,  OPENBLAS_CONST double  *x, OPENBLAS_CONST blasint incx,  OPENBLAS_CONST double beta,  double  *y, OPENBLAS_CONST blasint incy);
void cblas_cgemv64_(OPENBLAS_CONST enum CBLAS_ORDER order,  OPENBLAS_CONST enum CBLAS_TRANSPOSE trans,  OPENBLAS_CONST blasint m, OPENBLAS_CONST blasint n,
		 OPENBLAS_CONST void *alpha, OPENBLAS_CONST void  *a, OPENBLAS_CONST blasint lda,  OPENBLAS_CONST void  *x, OPENBLAS_CONST blasint incx,  OPENBLAS_CONST void *beta,  void  *y, OPENBLAS_CONST blasint incy);
void cblas_zgemv64_(OPENBLAS_CONST enum CBLAS_ORDER order,  OPENBLAS_CONST enum CBLAS_TRANSPOSE trans,  OPENBLAS_CONST blasint m, OPENBLAS_CONST blasint n,
		 OPENBLAS_CONST void *alpha, OPENBLAS_CONST void  *a, OPENBLAS_CONST blasint lda,  OPENBLAS_CONST void  *x, OPENBLAS_CONST blasint incx,  OPENBLAS_CONST void *beta,  void  *y, OPENBLAS_CONST blasint incy);

void cblas_sger64_ (OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N, OPENBLAS_CONST float   alpha, OPENBLAS_CONST float  *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST float  *Y, OPENBLAS_CONST blasint incY, float  *A, OPENBLAS_CONST blasint lda);
void cblas_dger64_ (OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N, OPENBLAS_CONST double  alpha, OPENBLAS_CONST double *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST double *Y, OPENBLAS_CONST blasint incY, double *A, OPENBLAS_CONST blasint lda);
void cblas_cgeru64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N, OPENBLAS_CONST void  *alpha, OPENBLAS_CONST void  *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST void  *Y, OPENBLAS_CONST blasint incY, void  *A, OPENBLAS_CONST blasint lda);
void cblas_cgerc64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N, OPENBLAS_CONST void  *alpha, OPENBLAS_CONST void  *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST void  *Y, OPENBLAS_CONST blasint incY, void  *A, OPENBLAS_CONST blasint lda);
void cblas_zgeru64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N, OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST void *Y, OPENBLAS_CONST blasint incY, void *A, OPENBLAS_CONST blasint lda);
void cblas_zgerc64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N, OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST void *Y, OPENBLAS_CONST blasint incY, void *A, OPENBLAS_CONST blasint lda);

void cblas_strsv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_DIAG Diag, OPENBLAS_CONST blasint N, OPENBLAS_CONST float *A, OPENBLAS_CONST blasint lda, float *X, OPENBLAS_CONST blasint incX);
void cblas_dtrsv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_DIAG Diag, OPENBLAS_CONST blasint N, OPENBLAS_CONST double *A, OPENBLAS_CONST blasint lda, double *X, OPENBLAS_CONST blasint incX);
void cblas_ctrsv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_DIAG Diag, OPENBLAS_CONST blasint N, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, void *X, OPENBLAS_CONST blasint incX);
void cblas_ztrsv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_DIAG Diag, OPENBLAS_CONST blasint N, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, void *X, OPENBLAS_CONST blasint incX);

void cblas_strmv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_DIAG Diag, OPENBLAS_CONST blasint N, OPENBLAS_CONST float *A, OPENBLAS_CONST blasint lda, float *X, OPENBLAS_CONST blasint incX);
void cblas_dtrmv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_DIAG Diag, OPENBLAS_CONST blasint N, OPENBLAS_CONST double *A, OPENBLAS_CONST blasint lda, double *X, OPENBLAS_CONST blasint incX);
void cblas_ctrmv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_DIAG Diag, OPENBLAS_CONST blasint N, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, void *X, OPENBLAS_CONST blasint incX);
void cblas_ztrmv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_DIAG Diag, OPENBLAS_CONST blasint N, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, void *X, OPENBLAS_CONST blasint incX);

void cblas_ssyr64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N, OPENBLAS_CONST float alpha, OPENBLAS_CONST float *X, OPENBLAS_CONST blasint incX, float *A, OPENBLAS_CONST blasint lda);
void cblas_dsyr64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N, OPENBLAS_CONST double alpha, OPENBLAS_CONST double *X, OPENBLAS_CONST blasint incX, double *A, OPENBLAS_CONST blasint lda);
void cblas_cher64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N, OPENBLAS_CONST float alpha, OPENBLAS_CONST void *X, OPENBLAS_CONST blasint incX, void *A, OPENBLAS_CONST blasint lda);
void cblas_zher64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N, OPENBLAS_CONST double alpha, OPENBLAS_CONST void *X, OPENBLAS_CONST blasint incX, void *A, OPENBLAS_CONST blasint lda);

void cblas_ssyr264_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo,OPENBLAS_CONST blasint N, OPENBLAS_CONST float alpha, OPENBLAS_CONST float *X,
                OPENBLAS_CONST blasint incX, OPENBLAS_CONST float *Y, OPENBLAS_CONST blasint incY, float *A, OPENBLAS_CONST blasint lda);
void cblas_dsyr264_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N, OPENBLAS_CONST double alpha, OPENBLAS_CONST double *X,
                OPENBLAS_CONST blasint incX, OPENBLAS_CONST double *Y, OPENBLAS_CONST blasint incY, double *A, OPENBLAS_CONST blasint lda);
void cblas_cher264_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N, OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *X, OPENBLAS_CONST blasint incX,
                OPENBLAS_CONST void *Y, OPENBLAS_CONST blasint incY, void *A, OPENBLAS_CONST blasint lda);
void cblas_zher264_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N, OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *X, OPENBLAS_CONST blasint incX,
                OPENBLAS_CONST void *Y, OPENBLAS_CONST blasint incY, void *A, OPENBLAS_CONST blasint lda);

void cblas_sgbmv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N,
                 OPENBLAS_CONST blasint KL, OPENBLAS_CONST blasint KU, OPENBLAS_CONST float alpha, OPENBLAS_CONST float *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST float *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST float beta, float *Y, OPENBLAS_CONST blasint incY);
void cblas_dgbmv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N,
                 OPENBLAS_CONST blasint KL, OPENBLAS_CONST blasint KU, OPENBLAS_CONST double alpha, OPENBLAS_CONST double *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST double *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST double beta, double *Y, OPENBLAS_CONST blasint incY);
void cblas_cgbmv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N,
                 OPENBLAS_CONST blasint KL, OPENBLAS_CONST blasint KU, OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST void *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST void *beta, void *Y, OPENBLAS_CONST blasint incY);
void cblas_zgbmv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N,
                 OPENBLAS_CONST blasint KL, OPENBLAS_CONST blasint KU, OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST void *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST void *beta, void *Y, OPENBLAS_CONST blasint incY);

void cblas_ssbmv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K, OPENBLAS_CONST float alpha, OPENBLAS_CONST float *A,
                 OPENBLAS_CONST blasint lda, OPENBLAS_CONST float *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST float beta, float *Y, OPENBLAS_CONST blasint incY);
void cblas_dsbmv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K, OPENBLAS_CONST double alpha, OPENBLAS_CONST double *A,
                 OPENBLAS_CONST blasint lda, OPENBLAS_CONST double *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST double beta, double *Y, OPENBLAS_CONST blasint incY);


void cblas_stbmv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_DIAG Diag,
                 OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K, OPENBLAS_CONST float *A, OPENBLAS_CONST blasint lda, float *X, OPENBLAS_CONST blasint incX);
void cblas_dtbmv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_DIAG Diag,
                 OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K, OPENBLAS_CONST double *A, OPENBLAS_CONST blasint lda, double *X, OPENBLAS_CONST blasint incX);
void cblas_ctbmv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_DIAG Diag,
                 OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, void *X, OPENBLAS_CONST blasint incX);
void cblas_ztbmv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_DIAG Diag,
                 OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, void *X, OPENBLAS_CONST blasint incX);

void cblas_stbsv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_DIAG Diag,
                 OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K, OPENBLAS_CONST float *A, OPENBLAS_CONST blasint lda, float *X, OPENBLAS_CONST blasint incX);
void cblas_dtbsv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_DIAG Diag,
                 OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K, OPENBLAS_CONST double *A, OPENBLAS_CONST blasint lda, double *X, OPENBLAS_CONST blasint incX);
void cblas_ctbsv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_DIAG Diag,
                 OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, void *X, OPENBLAS_CONST blasint incX);
void cblas_ztbsv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_DIAG Diag,
                 OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, void *X, OPENBLAS_CONST blasint incX);

void cblas_stpmv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_DIAG Diag,
                 OPENBLAS_CONST blasint N, OPENBLAS_CONST float *Ap, float *X, OPENBLAS_CONST blasint incX);
void cblas_dtpmv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_DIAG Diag,
                 OPENBLAS_CONST blasint N, OPENBLAS_CONST double *Ap, double *X, OPENBLAS_CONST blasint incX);
void cblas_ctpmv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_DIAG Diag,
                 OPENBLAS_CONST blasint N, OPENBLAS_CONST void *Ap, void *X, OPENBLAS_CONST blasint incX);
void cblas_ztpmv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_DIAG Diag,
                 OPENBLAS_CONST blasint N, OPENBLAS_CONST void *Ap, void *X, OPENBLAS_CONST blasint incX);

void cblas_stpsv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_DIAG Diag,
                 OPENBLAS_CONST blasint N, OPENBLAS_CONST float *Ap, float *X, OPENBLAS_CONST blasint incX);
void cblas_dtpsv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_DIAG Diag,
                 OPENBLAS_CONST blasint N, OPENBLAS_CONST double *Ap, double *X, OPENBLAS_CONST blasint incX);
void cblas_ctpsv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_DIAG Diag,
                 OPENBLAS_CONST blasint N, OPENBLAS_CONST void *Ap, void *X, OPENBLAS_CONST blasint incX);
void cblas_ztpsv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_DIAG Diag,
                 OPENBLAS_CONST blasint N, OPENBLAS_CONST void *Ap, void *X, OPENBLAS_CONST blasint incX);

void cblas_ssymv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N, OPENBLAS_CONST float alpha, OPENBLAS_CONST float *A,
                 OPENBLAS_CONST blasint lda, OPENBLAS_CONST float *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST float beta, float *Y, OPENBLAS_CONST blasint incY);
void cblas_dsymv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N, OPENBLAS_CONST double alpha, OPENBLAS_CONST double *A,
                 OPENBLAS_CONST blasint lda, OPENBLAS_CONST double *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST double beta, double *Y, OPENBLAS_CONST blasint incY);
void cblas_chemv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N, OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *A,
                 OPENBLAS_CONST blasint lda, OPENBLAS_CONST void *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST void *beta, void *Y, OPENBLAS_CONST blasint incY);
void cblas_zhemv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N, OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *A,
                 OPENBLAS_CONST blasint lda, OPENBLAS_CONST void *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST void *beta, void *Y, OPENBLAS_CONST blasint incY);


void cblas_sspmv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N, OPENBLAS_CONST float alpha, OPENBLAS_CONST float *Ap,
                 OPENBLAS_CONST float *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST float beta, float *Y, OPENBLAS_CONST blasint incY);
void cblas_dspmv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N, OPENBLAS_CONST double alpha, OPENBLAS_CONST double *Ap,
                 OPENBLAS_CONST double *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST double beta, double *Y, OPENBLAS_CONST blasint incY);

void cblas_sspr64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N, OPENBLAS_CONST float alpha, OPENBLAS_CONST float *X, OPENBLAS_CONST blasint incX, float *Ap);
void cblas_dspr64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N, OPENBLAS_CONST double alpha, OPENBLAS_CONST double *X, OPENBLAS_CONST blasint incX, double *Ap);

void cblas_chpr64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N, OPENBLAS_CONST float alpha, OPENBLAS_CONST void *X, OPENBLAS_CONST blasint incX, void *A);
void cblas_zhpr64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N, OPENBLAS_CONST double alpha, OPENBLAS_CONST void *X,OPENBLAS_CONST blasint incX, void *A);

void cblas_sspr264_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N, OPENBLAS_CONST float alpha, OPENBLAS_CONST float *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST float *Y, OPENBLAS_CONST blasint incY, float *A);
void cblas_dspr264_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N, OPENBLAS_CONST double alpha, OPENBLAS_CONST double *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST double *Y, OPENBLAS_CONST blasint incY, double *A);
void cblas_chpr264_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N, OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST void *Y, OPENBLAS_CONST blasint incY, void *Ap);
void cblas_zhpr264_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N, OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST void *Y, OPENBLAS_CONST blasint incY, void *Ap);

void cblas_chbmv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K,
		 OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST void *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST void *beta, void *Y, OPENBLAS_CONST blasint incY);
void cblas_zhbmv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K,
		 OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST void *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST void *beta, void *Y, OPENBLAS_CONST blasint incY);

void cblas_chpmv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N,
		 OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *Ap, OPENBLAS_CONST void *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST void *beta, void *Y, OPENBLAS_CONST blasint incY);
void cblas_zhpmv64_(OPENBLAS_CONST enum CBLAS_ORDER order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint N,
		 OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *Ap, OPENBLAS_CONST void *X, OPENBLAS_CONST blasint incX, OPENBLAS_CONST void *beta, void *Y, OPENBLAS_CONST blasint incY);

void cblas_sgemm64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransB, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K,
		 OPENBLAS_CONST float alpha, OPENBLAS_CONST float *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST float *B, OPENBLAS_CONST blasint ldb, OPENBLAS_CONST float beta, float *C, OPENBLAS_CONST blasint ldc);
void cblas_dgemm64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransB, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K,
		 OPENBLAS_CONST double alpha, OPENBLAS_CONST double *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST double *B, OPENBLAS_CONST blasint ldb, OPENBLAS_CONST double beta, double *C, OPENBLAS_CONST blasint ldc);
void cblas_cgemm64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransB, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K,
		 OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST void *B, OPENBLAS_CONST blasint ldb, OPENBLAS_CONST void *beta, void *C, OPENBLAS_CONST blasint ldc);
void cblas_cgemm3m64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransB, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K,
		 OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST void *B, OPENBLAS_CONST blasint ldb, OPENBLAS_CONST void *beta, void *C, OPENBLAS_CONST blasint ldc);
void cblas_zgemm64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransB, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K,
		 OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST void *B, OPENBLAS_CONST blasint ldb, OPENBLAS_CONST void *beta, void *C, OPENBLAS_CONST blasint ldc);
void cblas_zgemm3m64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransB, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K,
		 OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST void *B, OPENBLAS_CONST blasint ldb, OPENBLAS_CONST void *beta, void *C, OPENBLAS_CONST blasint ldc);


void cblas_ssymm64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_SIDE Side, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N,
                 OPENBLAS_CONST float alpha, OPENBLAS_CONST float *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST float *B, OPENBLAS_CONST blasint ldb, OPENBLAS_CONST float beta, float *C, OPENBLAS_CONST blasint ldc);
void cblas_dsymm64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_SIDE Side, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N,
                 OPENBLAS_CONST double alpha, OPENBLAS_CONST double *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST double *B, OPENBLAS_CONST blasint ldb, OPENBLAS_CONST double beta, double *C, OPENBLAS_CONST blasint ldc);
void cblas_csymm64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_SIDE Side, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N,
                 OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST void *B, OPENBLAS_CONST blasint ldb, OPENBLAS_CONST void *beta, void *C, OPENBLAS_CONST blasint ldc);
void cblas_zsymm64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_SIDE Side, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N,
                 OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST void *B, OPENBLAS_CONST blasint ldb, OPENBLAS_CONST void *beta, void *C, OPENBLAS_CONST blasint ldc);

void cblas_ssyrk64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE Trans,
		 OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K, OPENBLAS_CONST float alpha, OPENBLAS_CONST float *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST float beta, float *C, OPENBLAS_CONST blasint ldc);
void cblas_dsyrk64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE Trans,
		 OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K, OPENBLAS_CONST double alpha, OPENBLAS_CONST double *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST double beta, double *C, OPENBLAS_CONST blasint ldc);
void cblas_csyrk64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE Trans,
		 OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K, OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST void *beta, void *C, OPENBLAS_CONST blasint ldc);
void cblas_zsyrk64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE Trans,
		 OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K, OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST void *beta, void *C, OPENBLAS_CONST blasint ldc);

void cblas_ssyr2k64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE Trans,
		  OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K, OPENBLAS_CONST float alpha, OPENBLAS_CONST float *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST float *B, OPENBLAS_CONST blasint ldb, OPENBLAS_CONST float beta, float *C, OPENBLAS_CONST blasint ldc);
void cblas_dsyr2k64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE Trans,
		  OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K, OPENBLAS_CONST double alpha, OPENBLAS_CONST double *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST double *B, OPENBLAS_CONST blasint ldb, OPENBLAS_CONST double beta, double *C, OPENBLAS_CONST blasint ldc);
void cblas_csyr2k64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE Trans,
		  OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K, OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST void *B, OPENBLAS_CONST blasint ldb, OPENBLAS_CONST void *beta, void *C, OPENBLAS_CONST blasint ldc);
void cblas_zsyr2k64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE Trans,
		  OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K, OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST void *B, OPENBLAS_CONST blasint ldb, OPENBLAS_CONST void *beta, void *C, OPENBLAS_CONST blasint ldc);

void cblas_strmm64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_SIDE Side, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA,
                 OPENBLAS_CONST enum CBLAS_DIAG Diag, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N, OPENBLAS_CONST float alpha, OPENBLAS_CONST float *A, OPENBLAS_CONST blasint lda, float *B, OPENBLAS_CONST blasint ldb);
void cblas_dtrmm64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_SIDE Side, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA,
                 OPENBLAS_CONST enum CBLAS_DIAG Diag, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N, OPENBLAS_CONST double alpha, OPENBLAS_CONST double *A, OPENBLAS_CONST blasint lda, double *B, OPENBLAS_CONST blasint ldb);
void cblas_ctrmm64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_SIDE Side, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA,
                 OPENBLAS_CONST enum CBLAS_DIAG Diag, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N, OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, void *B, OPENBLAS_CONST blasint ldb);
void cblas_ztrmm64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_SIDE Side, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA,
                 OPENBLAS_CONST enum CBLAS_DIAG Diag, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N, OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, void *B, OPENBLAS_CONST blasint ldb);

void cblas_strsm64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_SIDE Side, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA,
                 OPENBLAS_CONST enum CBLAS_DIAG Diag, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N, OPENBLAS_CONST float alpha, OPENBLAS_CONST float *A, OPENBLAS_CONST blasint lda, float *B, OPENBLAS_CONST blasint ldb);
void cblas_dtrsm64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_SIDE Side, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA,
                 OPENBLAS_CONST enum CBLAS_DIAG Diag, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N, OPENBLAS_CONST double alpha, OPENBLAS_CONST double *A, OPENBLAS_CONST blasint lda, double *B, OPENBLAS_CONST blasint ldb);
void cblas_ctrsm64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_SIDE Side, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA,
                 OPENBLAS_CONST enum CBLAS_DIAG Diag, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N, OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, void *B, OPENBLAS_CONST blasint ldb);
void cblas_ztrsm64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_SIDE Side, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA,
                 OPENBLAS_CONST enum CBLAS_DIAG Diag, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N, OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, void *B, OPENBLAS_CONST blasint ldb);

void cblas_chemm64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_SIDE Side, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N,
                 OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST void *B, OPENBLAS_CONST blasint ldb, OPENBLAS_CONST void *beta, void *C, OPENBLAS_CONST blasint ldc);
void cblas_zhemm64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_SIDE Side, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST blasint M, OPENBLAS_CONST blasint N,
                 OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST void *B, OPENBLAS_CONST blasint ldb, OPENBLAS_CONST void *beta, void *C, OPENBLAS_CONST blasint ldc);

void cblas_cherk64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE Trans, OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K,
                 OPENBLAS_CONST float alpha, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST float beta, void *C, OPENBLAS_CONST blasint ldc);
void cblas_zherk64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE Trans, OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K,
                 OPENBLAS_CONST double alpha, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST double beta, void *C, OPENBLAS_CONST blasint ldc);

void cblas_cher2k64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE Trans, OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K,
                  OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST void *B, OPENBLAS_CONST blasint ldb, OPENBLAS_CONST float beta, void *C, OPENBLAS_CONST blasint ldc);
void cblas_zher2k64_(OPENBLAS_CONST enum CBLAS_ORDER Order, OPENBLAS_CONST enum CBLAS_UPLO Uplo, OPENBLAS_CONST enum CBLAS_TRANSPOSE Trans, OPENBLAS_CONST blasint N, OPENBLAS_CONST blasint K,
                  OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *A, OPENBLAS_CONST blasint lda, OPENBLAS_CONST void *B, OPENBLAS_CONST blasint ldb, OPENBLAS_CONST double beta, void *C, OPENBLAS_CONST blasint ldc);

void cblas_xerbla64_(blasint p, char *rout, char *form, ...);

/*** BLAS extensions ***/

void cblas_saxpby64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST float alpha, OPENBLAS_CONST float *x, OPENBLAS_CONST blasint incx,OPENBLAS_CONST float beta, float *y, OPENBLAS_CONST blasint incy);

void cblas_daxpby64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST double alpha, OPENBLAS_CONST double *x, OPENBLAS_CONST blasint incx,OPENBLAS_CONST double beta, double *y, OPENBLAS_CONST blasint incy);

void cblas_caxpby64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *x, OPENBLAS_CONST blasint incx,OPENBLAS_CONST void *beta, void *y, OPENBLAS_CONST blasint incy);

void cblas_zaxpby64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST void *alpha, OPENBLAS_CONST void *x, OPENBLAS_CONST blasint incx,OPENBLAS_CONST void *beta, void *y, OPENBLAS_CONST blasint incy);

void cblas_somatcopy64_(OPENBLAS_CONST enum CBLAS_ORDER CORDER, OPENBLAS_CONST enum CBLAS_TRANSPOSE CTRANS, OPENBLAS_CONST blasint crows, OPENBLAS_CONST blasint ccols, OPENBLAS_CONST float calpha, OPENBLAS_CONST float *a, 
		     OPENBLAS_CONST blasint clda, float *b, OPENBLAS_CONST blasint cldb); 
void cblas_domatcopy64_(OPENBLAS_CONST enum CBLAS_ORDER CORDER, OPENBLAS_CONST enum CBLAS_TRANSPOSE CTRANS, OPENBLAS_CONST blasint crows, OPENBLAS_CONST blasint ccols, OPENBLAS_CONST double calpha, OPENBLAS_CONST double *a,
		     OPENBLAS_CONST blasint clda, double *b, OPENBLAS_CONST blasint cldb); 
void cblas_comatcopy64_(OPENBLAS_CONST enum CBLAS_ORDER CORDER, OPENBLAS_CONST enum CBLAS_TRANSPOSE CTRANS, OPENBLAS_CONST blasint crows, OPENBLAS_CONST blasint ccols, OPENBLAS_CONST float* calpha, OPENBLAS_CONST float* a, 
		     OPENBLAS_CONST blasint clda, float*b, OPENBLAS_CONST blasint cldb); 
void cblas_zomatcopy64_(OPENBLAS_CONST enum CBLAS_ORDER CORDER, OPENBLAS_CONST enum CBLAS_TRANSPOSE CTRANS, OPENBLAS_CONST blasint crows, OPENBLAS_CONST blasint ccols, OPENBLAS_CONST double* calpha, OPENBLAS_CONST double* a, 
		     OPENBLAS_CONST blasint clda,  double *b, OPENBLAS_CONST blasint cldb); 

void cblas_simatcopy64_(OPENBLAS_CONST enum CBLAS_ORDER CORDER, OPENBLAS_CONST enum CBLAS_TRANSPOSE CTRANS, OPENBLAS_CONST blasint crows, OPENBLAS_CONST blasint ccols, OPENBLAS_CONST float calpha, float *a, 
		     OPENBLAS_CONST blasint clda, OPENBLAS_CONST blasint cldb); 
void cblas_dimatcopy64_(OPENBLAS_CONST enum CBLAS_ORDER CORDER, OPENBLAS_CONST enum CBLAS_TRANSPOSE CTRANS, OPENBLAS_CONST blasint crows, OPENBLAS_CONST blasint ccols, OPENBLAS_CONST double calpha, double *a,
		     OPENBLAS_CONST blasint clda, OPENBLAS_CONST blasint cldb); 
void cblas_cimatcopy64_(OPENBLAS_CONST enum CBLAS_ORDER CORDER, OPENBLAS_CONST enum CBLAS_TRANSPOSE CTRANS, OPENBLAS_CONST blasint crows, OPENBLAS_CONST blasint ccols, OPENBLAS_CONST float* calpha, float* a, 
		     OPENBLAS_CONST blasint clda, OPENBLAS_CONST blasint cldb); 
void cblas_zimatcopy64_(OPENBLAS_CONST enum CBLAS_ORDER CORDER, OPENBLAS_CONST enum CBLAS_TRANSPOSE CTRANS, OPENBLAS_CONST blasint crows, OPENBLAS_CONST blasint ccols, OPENBLAS_CONST double* calpha, double* a, 
		     OPENBLAS_CONST blasint clda, OPENBLAS_CONST blasint cldb); 

void cblas_sgeadd64_(OPENBLAS_CONST enum CBLAS_ORDER CORDER,OPENBLAS_CONST blasint crows, OPENBLAS_CONST blasint ccols, OPENBLAS_CONST float calpha, float *a, OPENBLAS_CONST blasint clda, OPENBLAS_CONST float cbeta, 
		  float *c, OPENBLAS_CONST blasint cldc); 
void cblas_dgeadd64_(OPENBLAS_CONST enum CBLAS_ORDER CORDER,OPENBLAS_CONST blasint crows, OPENBLAS_CONST blasint ccols, OPENBLAS_CONST double calpha, double *a, OPENBLAS_CONST blasint clda, OPENBLAS_CONST double cbeta, 
		  double *c, OPENBLAS_CONST blasint cldc); 
void cblas_cgeadd64_(OPENBLAS_CONST enum CBLAS_ORDER CORDER,OPENBLAS_CONST blasint crows, OPENBLAS_CONST blasint ccols, OPENBLAS_CONST float *calpha, float *a, OPENBLAS_CONST blasint clda, OPENBLAS_CONST float *cbeta, 
		  float *c, OPENBLAS_CONST blasint cldc); 
void cblas_zgeadd64_(OPENBLAS_CONST enum CBLAS_ORDER CORDER,OPENBLAS_CONST blasint crows, OPENBLAS_CONST blasint ccols, OPENBLAS_CONST double *calpha, double *a, OPENBLAS_CONST blasint clda, OPENBLAS_CONST double *cbeta, 
		  double *c, OPENBLAS_CONST blasint cldc); 

/*** BFLOAT16 and INT8 extensions ***/
/* convert float array to BFLOAT16 array by rounding */
void   cblas_sbstobf1664_(OPENBLAS_CONST blasint n, OPENBLAS_CONST float  *in, OPENBLAS_CONST blasint incin, bfloat16 *out, OPENBLAS_CONST blasint incout);
/* convert double array to BFLOAT16 array by rounding */
void   cblas_sbdtobf1664_(OPENBLAS_CONST blasint n, OPENBLAS_CONST double *in, OPENBLAS_CONST blasint incin, bfloat16 *out, OPENBLAS_CONST blasint incout);
/* convert BFLOAT16 array to float array */
void   cblas_sbf16tos64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST bfloat16 *in, OPENBLAS_CONST blasint incin, float  *out, OPENBLAS_CONST blasint incout);
/* convert BFLOAT16 array to double array */
void   cblas_dbf16tod64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST bfloat16 *in, OPENBLAS_CONST blasint incin, double *out, OPENBLAS_CONST blasint incout);
/* dot production of BFLOAT16 input arrays, and output as float */
float  cblas_sbdot64_(OPENBLAS_CONST blasint n, OPENBLAS_CONST bfloat16 *x, OPENBLAS_CONST blasint incx, OPENBLAS_CONST bfloat16 *y, OPENBLAS_CONST blasint incy);
void   cblas_sbgemv64_(OPENBLAS_CONST enum CBLAS_ORDER order,  OPENBLAS_CONST enum CBLAS_TRANSPOSE trans,  OPENBLAS_CONST blasint m, OPENBLAS_CONST blasint n, OPENBLAS_CONST float alpha, OPENBLAS_CONST bfloat16 *a, OPENBLAS_CONST blasint lda, OPENBLAS_CONST bfloat16 *x, OPENBLAS_CONST blasint incx, OPENBLAS_CONST float beta, float *y, OPENBLAS_CONST blasint incy);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif

#include <complex.h>
#include <stdint.h>
#include "libblastrampoline_internal.h"

// smax
extern double (*f2c_smax__addr)(const int32_t* n, const float* x, const int32_t* ix);
JL_HIDDEN float f2c_smax_(const int32_t* n, const float* x, const int32_t* ix) {
    return f2c_smax__addr(n, x, ix);
}
extern double (*f2c_smax_64__addr)(const int64_t* n, const float* x, const int64_t* ix);
JL_HIDDEN float f2c_smax_64_(const int64_t* n, const float* x, const int64_t* ix) {
	return f2c_smax_64__addr(n, x, ix);
}

// smin
extern double (*f2c_smin__addr)(const int32_t* n, const float* x, const int32_t* ix);
JL_HIDDEN float f2c_smin_(const int32_t* n, const float* x, const int32_t* ix) {
    return f2c_smin__addr(n, x, ix);
}
extern double (*f2c_smin_64__addr)(const int64_t* n, const float* x, const int64_t* ix);
JL_HIDDEN float f2c_smin_64_(const int64_t* n, const float* x, const int64_t* ix) {
	return f2c_smin_64__addr(n, x, ix);
}

// samax
extern double (*f2c_samax__addr)(const int32_t* n, const float* x, const int32_t* ix);
JL_HIDDEN float f2c_samax_(const int32_t* n, const float* x, const int32_t* ix) {
    return f2c_samax__addr(n, x, ix);
}
extern double (*f2c_samax_64__addr)(const int64_t* n, const float* x, const int64_t* ix);
JL_HIDDEN float f2c_samax_64_(const int64_t* n, const float* x, const int64_t* ix) {
	return f2c_samax_64__addr(n, x, ix);
}

// samin
extern double (*f2c_samin__addr)(const int32_t* n, const float* x, const int32_t* ix);
JL_HIDDEN float f2c_samin_(const int32_t* n, const float* x, const int32_t* ix) {
    return f2c_samin__addr(n, x, ix);
}
extern double (*f2c_samin_64__addr)(const int64_t* n, const float* x, const int64_t* ix);
JL_HIDDEN float f2c_samin_64_(const int64_t* n, const float* x, const int64_t* ix) {
	return f2c_samin_64__addr(n, x, ix);
}

// scamax
extern double (*f2c_scamax__addr)(const int32_t* n, const float complex* x, const int32_t* ix);
JL_HIDDEN float f2c_scamax_(const int32_t* n, const float complex* x, const int32_t* ix) {
    return f2c_scamax__addr(n, x, ix);
}
extern double (*f2c_scamax_64__addr)(const int64_t* n, const float complex* x, const int64_t* ix);
JL_HIDDEN float f2c_scamax_64_(const int64_t* n, const float complex* x, const int64_t* ix) {
	return f2c_scamax_64__addr(n, x, ix);
}

// scamin
extern double (*f2c_scamin__addr)(const int32_t* n, const float complex* x, const int32_t* ix);
JL_HIDDEN float f2c_scamin_(const int32_t* n, const float complex* x, const int32_t* ix) {
    return f2c_scamin__addr(n, x, ix);
}
extern double (*f2c_scamin_64__addr)(const int64_t* n, const float complex* x, const int64_t* ix);
JL_HIDDEN float f2c_scamin_64_(const int64_t* n, const float complex* x, const int64_t* ix) {
	return f2c_scamin_64__addr(n, x, ix);
}

// sdot_ and sdot_64_
extern double (*f2c_sdot__addr)(const int32_t* n, const float* x, const int32_t* ix, const float *y, const int32_t* iy);
JL_HIDDEN float f2c_sdot_(const int32_t* n, const float* x, const int32_t* ix, const float *y, const int32_t* iy) {
    return f2c_sdot__addr(n, x, ix, y, iy);
}
extern double (*f2c_sdot_64__addr)(const int64_t* n, const float* x, const int64_t* ix, const float *y, const int64_t* iy);
JL_HIDDEN float f2c_sdot_64_(const int64_t* n, const float* x, const int64_t* ix, const float *y, const int64_t* iy) {
	return f2c_sdot_64__addr(n, x, ix, y, iy);
}

// sdsdot_ and sdsdot_64_
extern double (*f2c_sdsdot__addr)(const int32_t* n, const float* a, const float* x, const int32_t* ix, const float *y, const int32_t* iy);
JL_HIDDEN float f2c_sdsdot_(const int32_t* n, const float* a, const float* x, const int32_t* ix, const float *y, const int32_t* iy) {
	return f2c_sdsdot__addr(n, a, x, ix, y, iy);
}
extern double (*f2c_sdsdot_64__addr)(const int64_t* n, const float* a, const float* x, const int64_t* ix, const float *y, const int64_t* iy);
JL_HIDDEN float f2c_sdsdot_64_(const int64_t* n, const float* a, const float* x, const int64_t* ix, const float *y, const int64_t* iy) {
	return f2c_sdsdot_64__addr(n, a, x, ix, y, iy);
}

// ssum
extern double (*f2c_ssum__addr)(const int32_t* n, const float* x, const int32_t* ix);
JL_HIDDEN float f2c_ssum_(const int32_t* n, const float* x, const int32_t* ix) {
    return f2c_ssum__addr(n, x, ix);
}
extern double (*f2c_ssum_64__addr)(const int64_t* n, const float* x, const int64_t* ix);
JL_HIDDEN float f2c_ssum_64_(const int64_t* n, const float* x, const int64_t* ix) {
	return f2c_ssum_64__addr(n, x, ix);
}

// sasum_ and sasum_64_
extern double (*f2c_sasum__addr)(const int32_t* n, const float* x, const int32_t* ix);
JL_HIDDEN float f2c_sasum_(const int32_t* n, const float* x, const int32_t* ix) {
	return f2c_sasum__addr(n, x, ix);
}
extern double (*f2c_sasum_64__addr)(const int64_t* n, const float* x, const int64_t* ix);
JL_HIDDEN float f2c_sasum_64_(const int64_t* n, const float* x, const int64_t* ix) {
    return f2c_sasum_64__addr(n, x, ix);
}

// scsum
extern double (*f2c_scsum__addr)(const int32_t* n, const float complex* x, const int32_t* ix);
JL_HIDDEN float f2c_scsum_(const int32_t* n, const float complex* x, const int32_t* ix) {
	return f2c_scsum__addr(n, x, ix);
}
extern double (*f2c_scsum_64__addr)(const int64_t* n, const float complex* x, const int64_t* ix);
JL_HIDDEN float f2c_scsum_64_(const int64_t* n, const float complex* x, const int64_t* ix) {
    return f2c_scsum_64__addr(n, x, ix);
}

// scasum_ and scasum_64_
extern double (*f2c_scasum__addr)(const int32_t* n, const float complex* x, const int32_t* ix);
JL_HIDDEN float f2c_scasum_(const int32_t* n, const float complex* x, const int32_t* ix) {
    return f2c_scasum__addr(n, x, ix);
}
extern double (*f2c_scasum_64__addr)(const int64_t* n, const float complex* x, const int64_t* ix);
JL_HIDDEN float f2c_scasum_64_(const int64_t* n, const float complex* x, const int64_t* ix) {
    return f2c_scasum_64__addr(n, x, ix);
}

// snrm2_ and snrm2_64_
extern double (*f2c_snrm2__addr)(const int32_t* n, const float* x, const int32_t* ix);
JL_HIDDEN float f2c_snrm2_(const int32_t* n, const float* x, const int32_t* ix) {
    return f2c_snrm2__addr(n, x, ix);
}
extern double (*f2c_snrm2_64__addr)(const int64_t* n, const float* x, const int64_t* ix);
JL_HIDDEN float f2c_snrm2_64_(const int64_t* n, const float* x, const int64_t* ix) {
    return f2c_snrm2_64__addr(n, x, ix);
}

// scnrm2_ and scnrm2_64_
extern double (*f2c_scnrm2__addr)(const int32_t* n, const float complex* x, const int32_t* ix);
JL_HIDDEN float f2c_scnrm2_(const int32_t* n, const float complex* x, const int32_t* ix) {
	return f2c_scnrm2__addr(n, x, ix);
}
extern double (*f2c_scnrm2_64__addr)(const int64_t* n, const float complex* x, const int64_t* ix);
JL_HIDDEN float f2c_scnrm2_64_(const int64_t* n, const float complex* x, const int64_t* ix) {
	return f2c_scnrm2_64__addr(n, x, ix);
}

// slamch
extern double (*f2c_slamch__addr)(const char * input);
JL_HIDDEN float f2c_slamch_(const char * input) {
	return f2c_slamch__addr(input);
}
extern double (*f2c_slamch_64__addr)(const char * input);
JL_HIDDEN float f2c_slamch_64_(const char * input) {
	return f2c_slamch_64__addr(input);
}

// slamc3
extern double (*f2c_slamc3__addr)(const float * x, const float * y);
JL_HIDDEN float f2c_slamc3_(const float * x, const float * y) {
	return f2c_slamc3__addr(x, y);
}
extern double (*f2c_slamc3_64__addr)(const float * x, const float * y);
JL_HIDDEN float f2c_slamc3_64_(const float * x, const float * y) {
	return f2c_slamc3_64__addr(x, y);
}


// cdotc
extern void (*f2c_cdotc__addr)(float complex *z, const int32_t *n, const float complex *x, const int32_t * ix, const float complex *y, const int32_t *iy);
JL_HIDDEN float complex f2c_cdotc_(const int32_t *n, const float complex *x, const int32_t * ix, const float complex *y, const int32_t *iy) {
	float complex z;
	f2c_cdotc__addr(&z, n, x, ix, y, iy);
	return z;
}
extern void (*f2c_cdotc_64__addr)(float complex *z, const int64_t *n, const float complex *x, const int64_t * ix, const float complex *y, const int64_t *iy);
JL_HIDDEN float complex f2c_cdotc_64_(const int64_t *n, const float complex *x, const int64_t * ix, const float complex *y, const int64_t *iy) {
	float complex z;
	f2c_cdotc_64__addr(&z, n, x, ix, y, iy);
	return z;
}

// cdotu
extern void (*f2c_cdotu__addr)(float complex *z, const int32_t *n, const float complex *x, const int32_t * ix, const float complex *y, const int32_t *iy);
float complex f2c_cdotu_(const int32_t* n, const float complex* x, const int32_t* ix, const float complex *y, const int32_t *iy)
{
	float complex z;
	f2c_cdotu__addr(&z, n, x, ix, y, iy);
	return z;
}
extern void (*f2c_cdotu_64__addr)(float complex *z, const int64_t *n, const float complex *x, const int64_t * ix, const float complex *y, const int64_t *iy);
float complex f2c_cdotu_64_(const int64_t* n, const float complex* x, const int64_t* ix, const float complex *y, const int64_t *iy)
{
	float complex z;
	f2c_cdotu_64__addr(&z, n, x, ix, y, iy);
	return z;
}

// zdotc
extern void (*f2c_zdotc__addr)(double complex *z, const int32_t *n, const double complex *x, const int32_t * ix, const double complex *y, const int32_t *iy);
JL_HIDDEN double complex f2c_zdotc_(const int32_t *n, const double complex *x, const int32_t * ix, const double complex *y, const int32_t *iy) {
	double complex z;
	f2c_zdotc__addr(&z, n, x, ix, y, iy);
	return z;
}
extern void (*f2c_zdotc_64__addr)(double complex *z, const int64_t *n, const double complex *x, const int64_t * ix, const double complex *y, const int64_t *iy);
JL_HIDDEN double complex f2c_zdotc_64_(const int64_t *n, const double complex *x, const int64_t * ix, const double complex *y, const int64_t *iy) {
	double complex z;
	f2c_zdotc_64__addr(&z, n, x, ix, y, iy);
	return z;
}

// zdotu
extern void (*f2c_zdotu__addr)(double complex *z, const int32_t *n, const double complex *x, const int32_t * ix, const double complex *y, const int32_t *iy);
double complex f2c_zdotu_(const int32_t* n, const double complex* x, const int32_t* ix, const double complex *y, const int32_t *iy)
{
	double complex z;
	f2c_zdotu__addr(n, x, ix, y, iy, &z);
	return z;
}
extern void (*f2c_zdotu_64__addr)(double complex *z, const int64_t *n, const double complex *x, const int64_t * ix, const double complex *y, const int64_t *iy);
double complex f2c_zdotu_64_(const int64_t* n, const double complex* x, const int64_t* ix, const double complex *y, const int64_t *iy)
{
	double complex z;
	f2c_zdotu_64__addr(n, x, ix, y, iy, &z);
	return z;
}

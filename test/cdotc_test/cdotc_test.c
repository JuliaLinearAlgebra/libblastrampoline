#include <stdio.h>
#include <stdint.h>
#include <complex.h>

#ifdef ILP64
#define MANGLE(x) x##64_
typedef int64_t blasint;
#else
#define MANGLE(x) x
typedef int32_t blasint;
#endif

extern void MANGLE(cblas_cdotc_sub)(blasint, float complex *, blasint, float complex *, blasint, float complex *);
extern float complex MANGLE(cdotc_)(blasint *, float complex *, blasint *, float complex *, blasint *);

#define N 2
int main()
{
    float complex A[N], B[N];

    // Initialize `A` with known values (transposed into FORTRAN ordering)
    A[0] = 3.1 + 1.4*I;
    A[1] = -1.0 + 1.2*I;

    // Initialize `b` with known values
    B[0] = 1.3 + 0.3*I;
    B[1] = -1.1 + -3.4*I;

    // Perform complex dot product
    blasint len = N;
    blasint inca = 1;
    blasint incb = 1;
    complex float C;
    MANGLE(cblas_cdotc_sub)(len, &A[0], inca, &B[0], incb, &C);

    // Print out C
    printf("C (cblas) is:   (%8.4f, %8.4f)\n", creal(C), cimag(C));

    // Do the same thing, but with the FORTRAN interface
    C = MANGLE(cdotc_)(&len, &A[0], &inca, &B[0], &incb);
    printf("C (fortran) is: (%8.4f, %8.4f)\n", creal(C), cimag(C));
}

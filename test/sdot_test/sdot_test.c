#include <stdio.h>
#include <stdint.h>

#ifdef ILP64
#define MANGLE(x) x##64_
typedef int64_t blasint;
#else
#define MANGLE(x) x
typedef int32_t blasint;
#endif

extern float MANGLE(sdot_)(blasint *, float *, blasint *, float *, blasint *);

#define N 3
int main()
{
    float A[N], B[N];

    // Initialize `A` with known values (transposed into FORTRAN ordering)
    A[0] = 3.1;
    A[1] = 1.0;
    A[2] = 3.4;

    // Initialize `b` with known values
    B[0] = -1.3;
    B[1] = -0.1;
    B[2] =  1.8;

    // Perform dot product
    blasint len = N;
    blasint inca = 1;
    blasint incb = 1;
    float C = MANGLE(sdot_)(&len, &A[0], &inca, &B[0], &incb);

    // Print out C
    printf("C is: %8.4f\n", C);
}

#include <stdio.h>
#include <stdint.h>

#ifdef ILP64
#define MANGLE(x) x##64_
typedef int64_t blasint;
#else
#define MANGLE(x) x
typedef int32_t blasint;
#endif

extern void MANGLE(sgesv_)(blasint *, blasint *, float *, blasint *, blasint *, float *, blasint *, blasint *);

#define N 3
int main()
{
    blasint pivot[N], ok;
    float A[N][N], b[N];

    // Initialize `A` with known values (transposed into FORTRAN ordering)
    A[0][0] =  3.1;  A[1][0] =  1.3;  A[2][0] = -5.7;
    A[0][1] =  1.0;  A[1][1] = -6.9;  A[2][1] =  5.8;
    A[0][2] =  3.4;  A[1][2] =  7.2;  A[2][2] = -8.8;

    // Initialize `b` with known values
    b[0] = -1.3;
    b[1] = -0.1;
    b[2] =  1.8;

    blasint c1 = N;
    blasint c2 = 1;

    // find solution using LAPACK routine SGESV, all the arguments have to
    // be pointers and you have to add an underscore to the routine name

    //  parameters in the order as they appear in the function call
    //  order of matrix A, number of right hand sides (b), matrix A,
    // leading dimension of A, array that records pivoting,
    // result vector b on entry, x on exit, leading dimension of b
    //  return value
    MANGLE(sgesv_)(&c1, &c2, &A[0][0], &c1, pivot, b, &c1, &ok);

    // Print out sum(b[j]^2)
    float sum = 0.0f;
    for (blasint j=0; j<N; j++) {
        sum += b[j]*b[j];
    }
    printf("||b||^2 is: %8.4f\n", sum);
}
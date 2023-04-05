#include <stdio.h>
#include <stdint.h>

#ifdef ILP64
#define MANGLE(x) x##64_
typedef int64_t blasint;
#else
#define MANGLE(x) x
typedef int32_t blasint;
#endif

extern blasint MANGLE(dpstrf_)(char *, blasint *, double *, blasint *, blasint *, blasint *, double *, double*, blasint *);

#define N 4
int main()
{
    blasint pivot[N];
    blasint info;
    double A[N][N];
    double work[2*N];
    blasint order = N;
    blasint rank = 0;
    blasint lda = N;
    blasint stride = 1;
    double tol = 0.0;
    

    // Initialize `A` with known values (transposed into FORTRAN ordering)
    A[0][0] =   3.4291134;  A[1][0] = -0.61112815; A[2][0] =  0.8155207;  A[3][0] = -0.9183448;
    A[0][1] =  -0.61112815; A[1][1] =  3.1250076;  A[2][1] = -0.44400603; A[3][1] =  0.97749346;
    A[0][2] =   0.8155207;  A[1][2] = -0.44400603; A[2][2] =  0.5413656;  A[3][2] =  0.53000593;
    A[0][3] =  -0.9183448;  A[1][3] =  0.97749346; A[2][3] =  0.53000593; A[3][3] =  5.108155;

    // find solution using LAPACK routine dpstrf, all the arguments have to
    // be pointers and you have to add an underscore to the routine name

    //  parameters in the order as they appear in the function call
    //  order of matrix A, number of right hand sides (b), matrix A,
    // leading dimension of A, array that records pivoting,
    // result vector b on entry, x on exit, leading dimension of b
    //  return value
    MANGLE(dpstrf_)("U", &order, &A[0][0], &lda, &pivot[0], &rank, &tol, &work[0], &info);
    if (info != 0) {
        printf("ERROR: info == %ld!\n", info);
        return 1;
    }

    // Print out diagonal of A
    printf("diag(A):");
    for (blasint j=0; j<N; j++) {
        printf(" %8.4f", A[j][j]);
    }
    printf("\n");
    return 0;
}

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef ILP64
#define MANGLE(x) x##64_
typedef int64_t blasint;
#else
#define MANGLE(x) x
typedef int32_t blasint;
#endif

#define MATRIX_IDX(n, i, j) j*n + i
#define MATRIX_ELEMENT(A, m, n, i, j) A[ MATRIX_IDX(m, i, j) ]
extern void MANGLE(dgemm_)(char *, char *, blasint *, blasint *, blasint *, double *, double *, blasint *, double *, blasint *, double *, double *, blasint *);

void init_matrix(double* A, int m, int n) {
   for (int j = 0; j < n; j++) {
      for (int i = 0; i < m; i++) {
         MATRIX_ELEMENT(A, m, n, i, j) = 1.0;
      }
   }
}

int matrix_all_equals(const double * A, int m, int n, double check_val) {
   for (int i = 0; i < m; i++) {
      for (int j = 0; j < n; j++) {
         double elem = MATRIX_ELEMENT(A, m, n, i, j);
         if (elem != check_val) {
             printf("[%d, %d] is %8.4f, not %8.4f!\n", i, j, elem, check_val);
             return 1;
         }
      }
   }
   return 0;
}

int main(int argc, char** argv) {
   blasint m = 2000;
   blasint n = 2000;
   blasint k = 2000;

   double * A = (double *)malloc(sizeof(double)* m * k);
   double * B = (double *)malloc(sizeof(double)* k * n);
   double * C = (double *)malloc(sizeof(double)* m * n);

   init_matrix(A, m, k);
   init_matrix(B, k, n);

   printf("Matrix A (%d x %d)\n", (int)m, (int)k);
   printf("Matrix B (%d x %d)\n", (int)k, (int)n);

   double alpha = 1.0, beta=0.0;
   char no = 'N';
   MANGLE(dgemm_)(&no, &no, &m, &n, &k, &alpha, A, &m, B, &k, &beta, C, &m);

   printf("Matrix C (%d x %d) = AB\n", (int)m, (int)n);
   double check_val = 2000.0;
   if (matrix_all_equals(C, m, n, check_val) == 0) {
       printf("All equal to %8.4f\n", check_val);
       return 0;
   } else {
       return 1;
   }
}

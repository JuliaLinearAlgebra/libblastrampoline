#include <stdio.h>
#include <stdint.h>

#ifdef ILP64
#define MANGLE(x) x##64_
typedef int64_t blasint;
#else
#define MANGLE(x) x
typedef int32_t blasint;
#endif

#define MATRIX_IDX(n, i, j) j*n + i
#define MATRIX_ELEMENT(A, m, n, i, j) A[ MATRIX_IDX(m, i, j) ]
extern void MANGLE(dgemmt_)(char *, char *, char *, blasint *, blasint *, double *, double *, blasint *, double *, blasint *, double *, double *, blasint *);

void init_matrix(double* A, int m, int n) {
   double element = 1.0;
   for (int j = 0; j < n; j++) {
      for (int i = 0; i < m; i++) {
         MATRIX_ELEMENT(A, m, n, i, j) = element;
         element *= 0.9;
      }
   }
}

float frobnorm2(const double * A, int m, int n) {
   float norm = 0.0f;
   for (int i = 0; i < m; i++) {
      for (int j = 0; j < n; j++) {
         float elem = MATRIX_ELEMENT(A, m, n, i, j);
         norm += elem*elem;
      }
   }
   return norm;
}

void print_matrix(const double* A, int m, int n) {
   for (int i = 0; i < m; i++) {
      for (int j = 0; j < n; j++) {
          printf("%8.4f", MATRIX_ELEMENT(A, m, n, i, j));
      }
      printf("\n");
   }
}

int main(int argc, char** argv) {
   blasint n = 4;
   blasint k = 5;

   double A[n * k];
   double B[k * n];
   double C[n * n];

   init_matrix(A, n, k);
   init_matrix(B, k, n);

   printf("Matrix A (%d x %d) is:\n", (int)n, (int)k);
   print_matrix(A, n, k);

   printf("\nMatrix B (%d x %d) is:\n", (int)k, (int)n);
   print_matrix(B, k, n);

   double alpha = 1.0, beta=0.0;
   char uploL = 'L';
   char uploU = 'U';
   char no = 'N';
   MANGLE(dgemmt_)(&uploL, &no, &no, &n, &k, &alpha, A, &n, B, &k, &beta, C, &n);
   MANGLE(dgemmt_)(&uploU, &no, &no, &n, &k, &alpha, A, &n, B, &k, &beta, C, &n);

   printf("\nMatrix C (%d x %d) = AB is:\n", (int)n, (int)n);
   print_matrix(C, n, n);
   printf("\n||C||^2 is: %8.4f\n", frobnorm2(C, n, n));

   return 0;
}

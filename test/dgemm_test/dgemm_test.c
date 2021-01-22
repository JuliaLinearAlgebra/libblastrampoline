#include <stdio.h>

// This is so that we don't have to fixup the symbol names with `objcopy` later
#ifdef ILP64
#define cblas_dgemm cblas_dgemm64_
#endif
#include <cblas.h>

#define MATRIX_IDX(n, i, j) j*n + i
#define MATRIX_ELEMENT(A, m, n, i, j) A[ MATRIX_IDX(m, i, j) ]

void init_matrix(double* A, int m, int n)
{
   double element = 1.0;
   for (int j = 0; j < n; j++)
   {
      for (int i = 0; i < m; i++)
      {
         MATRIX_ELEMENT(A, m, n, i, j) = element;
         element *= 0.9;
      }
   }
}

void print_matrix(const double* A, int m, int n)
{
   for (int i = 0; i < m; i++)
   {
      for (int j = 0; j < n; j++)
      {
          printf("%8.4f", MATRIX_ELEMENT(A, m, n, i, j));
      }
      printf("\n");
   }
}

int main(int argc, char** argv)
{
   int m = 3;
   int n = 4;
   int k = 5;

   double A[m * k];
   double B[k * n];
   double C[m * n];

   init_matrix(A, m, k);
   init_matrix(B, k, n);

   printf("Matrix A (%d x %d) is:\n", m, k);
   print_matrix(A, m, k);

   printf("\nMatrix B (%d x %d) is:\n", k, n);
   print_matrix(B, k, n);

   cblas_dgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, m, n, k, 1.0, A, m, B, k, 0.0, C, m);

   printf("\nMatrix C (%d x %d) = AB is:\n", m, n);
   print_matrix(C, m, n);

   return 0;
}
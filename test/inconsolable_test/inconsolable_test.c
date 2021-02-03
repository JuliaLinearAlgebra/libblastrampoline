#include <stdio.h>
#include <stdint.h>


#define MATRIX_IDX(n, i, j) j*n + i
#define MATRIX_ELEMENT(A, m, n, i, j) A[ MATRIX_IDX(m, i, j) ]

// 32-bit DGEMM
extern void dgemm_(char *, char *, int32_t *, int32_t *, int32_t *, double *, double *, int32_t *, double *, int32_t *, double *, double *, int32_t *);

// 64-bit SGESV
extern void sgesv_64_(int64_t *, int64_t *, float *, int64_t *, int64_t *, float *, int64_t *, int64_t *);

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

int main(int argc, char** argv) {
   // Invoke a 32-bit DGEMM
   {
      int32_t m = 3;
      int32_t n = 4;
      int32_t k = 5;

      double A[m * k];
      double B[k * n];
      double C[m * n];

      init_matrix(A, m, k);
      init_matrix(B, k, n);

      double alpha = 1.0, beta=0.0;
      char no = 'N';
      dgemm_(&no, &no, &m, &n, &k, &alpha, A, &m, B, &k, &beta, C, &m);
      printf("\n||C||^2 is: %8.4f\n", frobnorm2(C, m, n));
   }

   // Next, invoke a 64-bit SGESV on another matrix
   {
      int N = 3;
      int64_t pivot[N], ok;
      float A[N][N], b[N];

      A[0][0] =  3.1;  A[1][0] =  1.3;  A[2][0] = -5.7;
      A[0][1] =  1.0;  A[1][1] = -6.9;  A[2][1] =  5.8;
      A[0][2] =  3.4;  A[1][2] =  7.2;  A[2][2] = -8.8;

      b[0] = -1.3;
      b[1] = -0.1;
      b[2] =  1.8;

      int64_t c1 = N;
      int64_t c2 = 1;

      sgesv_64_(&c1, &c2, &A[0][0], &c1, pivot, b, &c1, &ok);

      // Print out sum(b[j]^2)
      float sum = 0.0f;
      for (int64_t j=0; j<N; j++) {
         sum += b[j]*b[j];
      }
      printf("||b||^2 is: %8.4f\n", sum);
   }
   return 0;
}
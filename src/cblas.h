// definitions that others can use when they build against us
typedef enum CBLAS_ORDER     {CblasRowMajor=101, CblasColMajor=102} CBLAS_ORDER;
typedef enum CBLAS_TRANSPOSE {CblasNoTrans=111, CblasTrans=112, CblasConjTrans=113, CblasConjNoTrans=114} CBLAS_TRANSPOSE;

typedef long long BLASLONG;
typedef BLASLONG blasint;
void cblas_dgemm64_(enum CBLAS_ORDER Order, enum CBLAS_TRANSPOSE TransA, enum CBLAS_TRANSPOSE TransB, blasint M, blasint N, blasint K,
		 double alpha, double *A, blasint lda, double *B, blasint ldb, double beta, double *C, blasint ldc);
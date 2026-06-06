#include <stdio.h>
#include <stdint.h>

extern int64_t isamax_64_(int64_t *, float *, int64_t *);

#define N 4
int main()
{
    int64_t n = 0xffffffff00000003;
    float X[3] = {1.0f, 2.0f, 1.0f};
    int64_t incx = 1;

    int64_t max_idx = isamax_64_(&n, X, &incx);
    printf("max_idx: %lld\n", max_idx);
    return 0;
}

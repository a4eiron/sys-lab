#include "common.h"
#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// clang-format off
__attribute__((noinline, optimize("no-tree-vectorize")))
void mul_scalar(const float *a, const float *b, float *c, int n) {
    for (int i = 0; i< n ;i++)
        c[i] = a[i] * b[i];
}

__attribute__((noinline)) 
void mul_avx2(const float *a, const float *b,  float *c, int n) {
    int i = 0;
    for (; i + 8 <= n; i += 8){
        __m256 va = _mm256_loadu_ps(a + i);
        __m256 vb = _mm256_loadu_ps(b + i);
        _mm256_storeu_ps(c + i, _mm256_mul_ps(va, vb));
    }

    for (; i < n ;i++)
        c[i] = a[i] * b[i];
}
// clang-format on

typedef void (*kernel)(const float *, const float *, float *, int);

static double bench(kernel k, const float *a, const float *b, float *c, int n,
                    long reps) {
    double best = 1e30;
    for (int i = 0; i < 3; i++) {
        double t0 = now();
        for (long r = 0; r < reps; r++) {
            k(a, b, c, n);
            __asm__ volatile("" ::: "memory");
        }

        double t = now() - t0;
        if (t < best)
            best = t;
    }
    return best;
}

int main(int argc, char **argv) {
    const long total = 1L << 28;
    printf("n,working_set_kib,scalar_gbps,avx2_gbps,speedup\n");

    for (int n = 1 << 10; n <= 1 << 24; n <<= 1) {
        float *a = aligned_alloc(64, (size_t)n * sizeof(float));
        float *b = aligned_alloc(64, (size_t)n * sizeof(float));
        float *c = aligned_alloc(64, (size_t)n * sizeof(float));
        for (int i = 0; i < n; i++) {
            a[i] = i * 0.5f;
            b[i] = i * 0.25f;
        }

        memset(c, 0, sizeof(float) * (size_t)n);

        long reps = total / n;
        if (reps < 1)
            reps = 1;

        double ts = bench(mul_scalar, a, b, c, n, reps);
        double tv = bench(mul_avx2, a, b, c, n, reps);

        double bytes = 12.0 * n * reps;
        printf("%d,%.1f,%.2f,%.2f,%.2f\n", n, 12.0 * n / 1024.0,
               bytes / ts / 1e9, bytes / tv / 1e9, ts / tv);
        fflush(stdout);
        free(a);
        free(b);
        free(c);
    }
    return 0;
}

// vector multiplication

#include "common.h"
#include <immintrin.h>
#include <math.h>
#include <stdio.h>

void mul_scalar(const float *a, const float *b, float *c, int n) {
    for (int i = 0; i < N; i++)
        c[i] = a[i] * b[i];
}

void mul_avx2(const float *a, const float *b, float *c, int n) {
    int i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);
        __m256 vb = _mm256_loadu_ps(b + i);
        __m256 vc = _mm256_mul_ps(va, vb);
        _mm256_storeu_ps(c + i, vc);
    }

    for (; i < n; i++)
        c[i] = a[i] * b[i];
}

int main() {

    float *a = malloc(N * sizeof(float));
    float *b = malloc(N * sizeof(float));
    float *c1 = malloc(N * sizeof(float));
    float *c2 = malloc(N * sizeof(float));

    for (int i = 0; i < N; i++) {
        a[i] = i * 0.5f;
        b[i] = i * 0.25f;
    }

    double t0 = now();

    for (int i = 0; i < REPS; i++) {
        mul_scalar(a, b, c1, N);
        __asm__ volatile("" ::: "memory");
    }

    double t1 = now();

    for (int i = 0; i < REPS; i++) {
        mul_avx2(a, b, c2, N);
        __asm__ volatile("" ::: "memory");
    }
    double t2 = now();

    for (int i = 0; i < N; i++) {
        if (fabsf(c1[i] - c2[i]) > 1e-5f) {
            printf("MISMATCH at %d\n", i);
            return 1;
        }
    }

    printf("results match\n");
    printf("scalar: %.2f ms\navx2: %.2f ms\n", (t1 - t0) * 1e3,
           (t2 - t1) * 1e3);
}

// vector addition

#include <immintrin.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N (1 << 24) // 16 M

void add_scalar(const float *a, const float *b, float *c, int n) {
    for (int i = 0; i < n; i++)
        c[i] = a[i] + b[i];
}

// _mm256 - register width
// add - operation
// ps - data type
void add_avx2(const float *a, const float *b, float *c, int n) {
    int i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i); // unaligned
        __m256 vb = _mm256_loadu_ps(b + i);
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(c + i, vc);
    }

    // handle leftover
    for (; i < n; i++)
        c[i] = a[i] + b[i];
}

static double now() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}

int main(int argc, char **argv) {
    float *a = malloc(N * sizeof(float));
    float *b = malloc(N * sizeof(float));
    float *c1 = malloc(N * sizeof(float));
    float *c2 = malloc(N * sizeof(float));

    for (int i = 0; i < N; i++) {
        a[i] = i * 0.5f;
        b[i] = i * 0.25f;
    }

    double t0 = now();

    add_scalar(a, b, c1, N);
    double t1 = now();

    add_avx2(a, b, c2, N);
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

    return 0;
}

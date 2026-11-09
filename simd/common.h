#ifndef COMMON_H
#define COMMON_H

#include <time.h>

// #define N (1 << 24) // 16 M

#define N 2048

#define REPS 200000

static double now() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}

#endif // COMMON_H

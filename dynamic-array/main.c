#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int *data;
  size_t len;
  size_t cap;
} IntArray;

int ia_init(IntArray *arr, size_t cap) {
  arr->data = NULL;
  arr->len = 0;
  arr->cap = 0;

  if (cap == 0)
    return 0;

  if (cap > SIZE_MAX / sizeof(int)) {
    errno = ENOMEM;
    return -1;
  }

  int *p = malloc(sizeof(int) * cap);
  if (!p)
    return -1;

  arr->data = p;
  arr->cap = cap;
  return 0;
}

int ia_push(IntArray *arr, int value) {
  if (arr->len == arr->cap) {
    size_t newcap = arr->cap ? arr->cap *= 2 : 1;
    if (newcap > SIZE_MAX / sizeof(int)) {
      errno = ENOMEM;
      return -1;
    }

    int *tmp = realloc(arr->data, newcap * sizeof(int));
    if (!tmp)
      return -1;

    arr->data = tmp;
    arr->cap = newcap;
  }

  arr->data[arr->len++] = value;
  return 0;
}

int ia_set(IntArray *arr, size_t i, int value) {
  if (i >= arr->len) {
    errno = EINVAL;
    return -1;
  }

  arr->data[i] = value;
  return 0;
}

int ia_get(const IntArray *arr, size_t i, int *out) {
  if (i >= arr->len) {
    errno = EINVAL;
    return -1;
  }

  *out = arr->data[i];
  return 0;
}

void ia_free(IntArray *arr) {
  if (arr->data != NULL) {
    free(arr->data);
    arr->data = NULL;
  }
  arr->len = 0;
  arr->cap = 0;
}

int main(int argc, char **argv) {
  IntArray arr;
  if (ia_init(&arr, 5) < 0) {
    perror("ia_init");
    ia_free(&arr);
    return 1;
  }

  printf("len: %ld, cap: %ld\n", arr.len, arr.cap);

  for (int i = 0; i < 8; i++) {
    ia_push(&arr, i + 1);
  }

  ia_set(&arr, 3, 400);

  for (int i = 0; i < arr.len; i++) {
    int v;
    if (ia_get(&arr, i, &v) < 0) {
      perror("ia_get");
      ia_free(&arr);
    }
    printf("%d\n", v);
  }

  printf("len: %ld, cap: %ld\n", arr.len, arr.cap);

  ia_free(&arr);
  return 0;
}

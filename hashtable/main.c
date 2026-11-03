#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct HashNode {
  char *key;
  int value;
  struct HashNode *next;
} HashNode;

typedef struct {
  HashNode **buckets;
  size_t cap;
  size_t size;
} HashTable;

// djb2 hash function
static unsigned long hash(const char *s, size_t cap) {
  unsigned long hash = 5381;
  int c;
  while ((c = *s++)) {
    hash = ((hash << 5) + hash) + c;
  }
  return hash % cap;
}

static char *dup_string(const char *s) {
  size_t n = strlen(s) + 1;
  char *copy = malloc(n);

  if (copy)
    memcpy(copy, s, n);
  return copy;
}

int ht_init(HashTable *ht, size_t cap) {
  if (!ht || cap == 0)
    return 0;

  ht->buckets = calloc(cap, sizeof(HashNode *)); // arr of pointers
  if (!ht->buckets)
    return 0;
  ht->size = 0;
  ht->cap = cap;
  return 1;
}

void ht_free(HashTable *ht) {
  if (!ht || !ht->buckets)
    return;

  for (size_t i = 0; i < ht->cap; i++) {
    HashNode *n = ht->buckets[i];
    while (n) {
      HashNode *next = n->next;
      free(n->key);
      free(n);
      n = next;
    }
  }
  free(ht->buckets);
  ht->buckets = NULL;
  ht->cap = 0;
  ht->size = 0;
}

int ht_insert(HashTable *ht, const char *key, int value) {
  if (!ht || !key)
    return 0;

  size_t idx = hash(key, ht->cap);

  // update inplace if key already exists
  for (HashNode *n = ht->buckets[idx]; n; n = n->next) {
    if (strcmp(n->key, key) == 0) {
      n->value = value;
      return 1;
    }
  }

  // new entry
  HashNode *n = (HashNode *)malloc(sizeof(HashNode));
  if (!n)
    return 0;

  n->key = dup_string(key);
  if (!n->key) {
    free(n);
    return 0;
  }

  n->value = value;
  n->next = ht->buckets[idx];
  ht->buckets[idx] = n;
  ht->size++;
  return 1;
}

int ht_get(const HashTable *ht, const char *key, int *out) {
  if (!ht || !key || !out)
    return 0;

  size_t idx = hash(key, ht->cap);
  for (HashNode *n = ht->buckets[idx]; n; n = n->next) {
    if (strcmp(n->key, key) == 0) {
      *out = n->value;
      return 1;
    }
  }
  return -1; // -1 on not found
}

int ht_remove(HashTable *ht, const char *key) {
  if (!ht || !key)
    return 0;

  size_t idx = hash(key, ht->cap);
  HashNode **pp = &ht->buckets[idx];

  while (*pp) {
    HashNode *n = *pp;
    if (strcmp(n->key, key) == 0) {
      *pp = n->next;
      free(n->next);
      free(n);
      ht->size--;
      return 1;
    }
    pp = &n->next;
  }
  return 0;
}

int main(int argc, char **argv) {
  HashTable ht;
  if (!ht_init(&ht, 16)) {
    fprintf(stderr, "init failed\n");
    return 1;
  }

  ht_insert(&ht, "apple", 42);
  ht_insert(&ht, "banana", 17);
  ht_insert(&ht, "carrot", 99);
  ht_insert(&ht, "apple", 100);

  int v;
  if (ht_get(&ht, "apple", &v))
    printf("apple = %d\n", v);
  if (ht_get(&ht, "banana", &v))
    printf("banana = %d\n", v);
  if (ht_get(&ht, "carrot", &v))
    printf("carrot = %d\n", v);

  ht_remove(&ht, "banana");
  if (ht_get(&ht, "banana", &v) < 0)
    printf("banana removed");

  ht_free(&ht);
  return 0;
}

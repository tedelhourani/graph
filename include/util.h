#ifndef GRAPH_UTIL_H
#define GRAPH_UTIL_H 1

#include "graph.h"

typedef int (*function)(struct graph *, ...);

struct hashtable {
  unsigned int size;
  unsigned int *slot_list_len;
  struct hashtable_slot *first_slot;
};

struct hashtable_slot {
  char *key;
  void *value;
  struct hashtable_slot *next;
};

int hashtable_allocate(unsigned int, struct hashtable **);
int hashtable_delete(char *, struct hashtable *);
int hashtable_free(struct hashtable **);
int hashtable_insert(char *, void *, struct hashtable *, struct hashtable_slot **);
int hashtable_lookup(char *, struct hashtable *, struct hashtable_slot **);
int hashtable_size(int);

#define MAX_LABEL_LEN 50
#define PATHNAME_MAX_LENGTH 200
#define HASHTABLE_MAX_SIZE 40
#define HASHTABLE_OVERALLOCATION_FACTOR 5

#define ADD_ALGORITHM(n, f)         \
  hashtable_insert(n, (function)f, algorithms, &slot);

#endif /* util.h */

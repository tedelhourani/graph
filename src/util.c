#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

int
hash_naive(char *name, unsigned int hashtable_max_size){
  unsigned int index = 0;
  int i;

  for (i = 0 ; name[i] != '\0'; i++)
    index += name[i] * 15;

  return index % hashtable_max_size;
}

int
hash_pjw(char *name, unsigned int hashtable_max_size){
  unsigned int index = 0, four_most_significant_bits;
  int i;

  for (i = 0 ; name[i] != '\0'; i++) {
    index = (index << 4) + name[i];
    if ((four_most_significant_bits = index & 0xf0000000)) {
      index = index ^ (four_most_significant_bits >> 24);
      index = index ^ four_most_significant_bits;
    }
  }

  return index % hashtable_max_size;
}

int
hash(char *name, unsigned int hashtable_max_size) {
  return hash_pjw(name, hashtable_max_size);
}

int
hashtable_size(int expected_number_of_occupied_slots) {
  return expected_number_of_occupied_slots * HASHTABLE_OVERALLOCATION_FACTOR;
}

int
hashtable_allocate(unsigned int expected_slot_count,
                   struct hashtable **ht) {
  struct hashtable_slot *ht_slots;

  *ht = malloc(sizeof(struct hashtable));
  if (*ht == NULL) {
    fprintf(stderr, "Failed to allocated hashtable (out of memory).\n");
    return 1;
  }

  ht_slots = malloc(hashtable_size(expected_slot_count)
                   * sizeof(struct hashtable_slot));
  if (ht_slots == NULL) {
    fprintf(stderr, "Failed to allocated hashtable (out of memory).\n");
    return 1;
  }

  (*ht)->slot_list_len = calloc(hashtable_size(expected_slot_count),
                                sizeof(unsigned int));

  (*ht)->size = hashtable_size(expected_slot_count);
  (*ht)->first_slot = ht_slots;

  return 0;
}

int
hashtable_insert(char *key,
                 void *value,
                 struct hashtable *ht,
                 struct hashtable_slot **new_slot) {
  int i;
  struct hashtable_slot *s;

  if (ht == NULL || ht->first_slot == NULL)
    return 1;

  assert(ht->size > 0);

  i = hash(key, ht->size);
  s = ht->first_slot + i;

  if (s == NULL) {
    fprintf(stderr, "Hash value %d is greater than size of hashtable %d.\n",
            i, ht->size);
    return 1;
  }

  if (s->key == NULL) {
    *new_slot = s;
    assert(s->next == NULL);
  }
  else
    if (!strcmp(s->key, key)) {
      fprintf(stderr, "Key %s already present in table.\n", key);
      *new_slot = s;
      return 1;      
    }
    else {
      //s->l++;
      ht->slot_list_len[i] += 1;

      while (s->next != NULL) {
        s = s->next;
        if (!strcmp(s->key, key)) {
          fprintf(stderr, "Key %s is already present in the table.\n", key);
          *new_slot = s;
          return 1;
        }
      }
      
      *new_slot = malloc(sizeof(struct hashtable_slot));
      if (*new_slot == NULL) {
        fprintf(stderr, "Failed to allocated hashtable slot (out of memory).\n");
        return -1;
      }
      s->next = *new_slot;
    }

  (*new_slot)->key = malloc(strlen(key)+1);
  strncpy((*new_slot)->key, key, strlen(key));

  (*new_slot)->value = value;

  return 0;
}

int
hashtable_lookup(char *key,
                 struct hashtable *ht,
                 struct hashtable_slot **slot) {
  int i;
  struct hashtable_slot *s;

  *slot = NULL;

  if (ht == NULL || ht->first_slot == NULL)
    return 1;

  assert(ht->size > 0);

  i = hash(key, ht->size);
  s = ht->first_slot + i;

  if (s->key == NULL)
    return 1;
  printf("%s hashes to %d\n", key, i);

  while (strcmp(s->key, key)) {
    s = s->next;
    if (s == NULL)
      return 1;
  }

  *slot = s;

  return 0;
}

int
hashtable_delete(char *key,
                 struct hashtable *ht) {
  int i;
  struct hashtable_slot *s;
  struct hashtable_slot *prev_slot, *tmp_slot;

  printf("Attempting to delete %s\n", key);

  if (ht == NULL || ht->first_slot == NULL)
    return 1;

  assert(ht->size > 0);

  i = hash(key, ht->size);
  s = ht->first_slot + i;

  printf("%s hashes to index %d\n", key, i);
  if (s->key == NULL)
    return 1;

  if (!strcmp(s->key, key)) {
    free(s->key);
    if (s->next == NULL) {
      printf("%s is at index %d and the list is empty\n", key, i);
      s->key = NULL;
    }
    else {
      printf("%s is at index %d and the list has %d\n", key, i, ht->slot_list_len[i]);
      //->next->l = s->l - 1;
      ht->slot_list_len[i] -= 1;
      tmp_slot = s->next;
      memcpy(s, s->next, sizeof(struct hashtable_slot));
      free(tmp_slot);
      printf("%s is the new key at index %d and the list has %d\n", s->key, i, ht->slot_list_len[i]);
    }
  }
  else {
    while (strcmp(s->key, key)) {
      prev_slot = s;
      s = s->next;
      if (s == NULL)
        return -1;
    }
    prev_slot->next = s->next;
    free(s->key);
    free(s);
  }

  return 0;
}

int
hashtable_free(struct hashtable **ht) {
  struct hashtable_slot *ht_ptr, *ht_ptr_next;
  int i;

  if (*ht == NULL || (*ht)->first_slot == NULL)
    return 0;

  for (i = 0; i < (*ht)->size; i++) {
    ht_ptr = ((*ht)->first_slot + i)->next;
    while (ht_ptr != NULL )
      {
        ht_ptr_next = ht_ptr->next;
        free(ht_ptr);
        ht_ptr = ht_ptr_next;
      }
  }

  free((*ht)->first_slot);
  free(*ht);
  ht = NULL;
  return 0;
}

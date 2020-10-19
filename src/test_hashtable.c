#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "graph.h"
#include "util.h"
#include "path.h"

int
test_main(int argc, char *argv[]) {
  unsigned *value;
  int index = 1;
  int expected_slot_count = 130000;
  struct hashtable *char_count;
  struct hashtable_slot *new;

  FILE *fp;
  char *word = NULL;
  size_t len = 0;
  ssize_t byte_count = 0;

  if (hashtable_allocate(expected_slot_count, &char_count)) {
    fprintf(stderr, "cannot allocate state hashtable\n");
    return 1;
  }

  fp = fopen("words.txt", "r");
  while ((byte_count = getline(&word, &len, fp)) != -1) {
    word[byte_count-1] = '\0';
    value = (unsigned int *)malloc(sizeof(unsigned int));
    *value = index;
    hashtable_insert(word, value, char_count, &new);
    /* new->value = (unsigned int *)malloc(sizeof(unsigned int)); */
    /* *((unsigned int *)new->value) = index; */
    index += 1;
  }
  fclose(fp);

  /* for (int i=0; i < hashtable_size(130000); i++) */
  /*   if ( (char_count + i)->l) */
  /*     printf("%d\n", (char_count+i)->l); */

  char *buffer = "grease";
  hashtable_delete(buffer, char_count);


  hashtable_lookup("Ohatchee", char_count, &new);
  printf("Order of \"%s\" is %d\n", new->key, *((unsigned int *)new->value));
  hashtable_delete("Ohatchee", char_count);

  hashtable_lookup("Revolite", char_count, &new);
  printf("Order of \"%s\" is %d\n", new->key, *((unsigned int *)new->value));
  hashtable_delete("Revolite", char_count);
  

  hashtable_delete("gklrt;wjgortwhgortwnRevolite", char_count);

  
  hashtable_lookup(buffer, char_count, &new);
  if (new == NULL) {
    printf("\"%s\" not found\n", buffer);
  } else {
    printf("Order of \"%s\" is %d\n", buffer, *((unsigned int *)new->value));
  }

  hashtable_free(&char_count);

  /* hashtable_insert(char *str, void *hashtable, void **slot, */
  /*                  unsigned int slot_size, unsigned int hashtable_size) { */
  /*hashtable_insert("hello", state, hashtable_size(expected_slot_count), &new);*/

  return 0;
}

#include <config.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "graph.h"

typedef int (*algorithm)(graph *, ...);

struct element {
  char *name;
  algorithm alg;
  struct element *next;
};

#define MAX_FUNC_NAME_SIZE 20
#define HASHTABLE_MAX_SIZE 2000

int
dalg1(int x) {
  return x * 2;
}

int
dalg2(int x, int y) {
  return x + y;
}

int
hash(char *algorithm){
  unsigned int index = 0;
  int i;
  
  for (i = 0 ; algorithm[i] != '\0'; i++)
    index += algorithm[i] * 15;

  return index % HASHTABLE_MAX_SIZE;
}

int
main(int argc, char **argv)
{
  int i, j;
  int counter = 0;

  /*char func[MAX_FUNC_NAME_SIZE];
  strcpy(func, argv[1]);
  strcat(func, "_");
  strcat(func, argv[2]);*/

  /*printf("%s\n", func);*/

  /* create a lookup table that maps function 
     names to function pointers */
  static struct element hashtable[HASHTABLE_MAX_SIZE];

  for (i = 0 ; i < HASHTABLE_MAX_SIZE; i++)
    {
      if ( hashtable[i] == NULL )
        counter += 1;
    }
  printf("Count is %d\n", counter);


  i = hash("dalg1");
  if ( hashtable[i] != NULL )
    printf("Collision, cannot insert dalg1\n");  
  else
    {
      hashtable[i] = malloc(sizeof(struct element));
      hashtable[i]->alg = dalg1;
      hashtable[i]->next = NULL;
      printf("Inserted dalg1 at %d\n", i);
    }

  j = hash("dalg2");
  if ( hashtable[j] != NULL )
    printf("Collision, cannot insert dalg2\n");  
  else
    {
      hashtable[j] = malloc(sizeof(struct element));
      hashtable[j]->alg = dalg2;
      hashtable[j]->next = NULL;
      printf("Inserted dalg2 at %d\n", j);
    }

  printf("i --> %d\n", (hashtable[i]->alg)(10));
  printf("j --> %d\n", (hashtable[j]->alg)(1, 8, 5));
  
  return 0;
}

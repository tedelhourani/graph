#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "graph.h"

int
maximal_path(struct graph *graph,
             struct vertex *start,
             struct edge_list **path) {
  struct edge_list *path_tail = NULL;
  struct edge_list *path_next = NULL;
  struct vertex *vertex;
  struct edge *edge;

  *path = NULL;

  if (start == NULL)
    return 0;

  vertex = start;
  while (vertex->edge_list_ptr != NULL)
    {
      edge = vertex->edge_list_ptr->edge;
      if (!edge->flag)
        {
          edge->flag = true;
          path_next = malloc(sizeof(struct edge_list));
          path_next->edge = vertex->edge_list_ptr->edge;
          path_next->next = NULL;

          if ( *path == NULL )
              *path = path_next;
          else
              path_tail->next = path_next;

          path_tail = path_next;

          if (vertex == edge->x)
            vertex = edge->y;
          else
            vertex = edge->x;
        }
      else
        vertex->edge_list_ptr = vertex->edge_list_ptr->next;
    }

  return 0;
}

/*
void
breadth_first_search() { }
*/


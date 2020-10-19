#ifndef GRAPH_H
#define GRAPH_H 1

#include <stdbool.h>

struct edge_list {
  struct edge *edge;
  struct edge_list *next;
};

struct vertex {
  char* name;
  struct edge_list *edges;
  struct edge_list *edge_list_ptr;
  int weight;
  struct vertex *next;
};

struct edge {
  char *name;
  struct vertex *x;
  struct vertex *y;
  bool flag;
  bool directed;
  int weight;
  struct edge *next;
};

struct graph {
  char *name;
  int num_vertices;
  int num_edges;
  struct vertex *vertices;
  struct edge *edges;
};

int load_graph(char *, struct graph **);
int list_edge_list(struct edge_list *);
int free_graph(struct graph *);

#define JGF ".json"
#define JGF_PATH "jgf/"
#define DEFAULT_GRAPH "simple"

#endif /* graph.h */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <json-c/json.h>
//#include <json-c/json_object.h>

#include "graph.h"
#include "util.h"

int
read_file_into_string(char *name, char **file_str) {
  FILE *file;
  struct stat file_stats;

  char pathname[PATHNAME_MAX_LENGTH+1] = "";

  strncat(pathname, JGF_PATH, PATHNAME_MAX_LENGTH);
  strncat(pathname, name, PATHNAME_MAX_LENGTH - strlen(pathname) - 1);
  strncat(pathname, JGF, PATHNAME_MAX_LENGTH - strlen(pathname) -1);
  
  if (stat(pathname, &file_stats)) {
    perror("failed to obtain file stats");
    return 1;
  }

  (*file_str) = calloc(file_stats.st_size + 1, sizeof(char));
  if (*file_str == NULL) {
    fprintf(stderr, "failed to allocated heap space to fit %s\n", pathname);
    return 1;
  }

  file = fopen(pathname, "r");
  fread((*file_str), sizeof(char), file_stats.st_size, file);
  fclose(file);

  return 0;
}

int
update_vertex_edge_list(struct vertex *v, struct edge *e) {
  struct edge_list *list = v->edges;
  struct edge_list *new = malloc(sizeof(struct edge_list));

  if (new == NULL) {
    fprintf(stderr, "Failed to allocated edge_list element (out of memory).\n");
    return 1;
  }

  new->edge = e;
  new->next = NULL;

  if (list == NULL) {
    list = new;
    v->edge_list_ptr = v->edges;
    return 0;
  }

  while (list->next != NULL)
    list = list->next;
  list->next = new;

  return 0;
}

int
load_vertices(struct json_object *nodes,
              struct hashtable **vertices,
              int *num_vertices) {
  struct hashtable_slot *new_vertex;
  struct json_object *obj;
  struct vertex *v;
  char label[MAX_LABEL_LEN] = {'\0'};

  *num_vertices = json_object_object_length(json_object_get(nodes));
  hashtable_allocate(*num_vertices, vertices);

  json_object_object_foreach(nodes, key, val) {
    obj = json_object_get(val);
    json_object_object_foreach(obj, node_key, node_val) {
      if (!strcmp(node_key, "label"))
        strcpy(label, json_object_to_json_string_ext(node_val, JSON_C_TO_STRING_PLAIN));
    }
    v = malloc(sizeof(struct vertex));
    v->name = malloc(strlen(label)+1);
    strncpy(v->name, label, strlen(label));
    hashtable_insert(label, v, *vertices, &new_vertex);
    memset(label,0,MAX_LABEL_LEN);
  }

  return 0;
}

int
load_edges(struct json_object *edges_json_object,
           struct hashtable **edges, int *num_edges, struct hashtable *vertices) {
  int i;
  struct json_object *obj;
  struct edge *e;
  struct hashtable_slot **new_edge;
  struct hashtable_slot *vertex = NULL;
  char label[MAX_LABEL_LEN] = {'\0'};
  char source[MAX_LABEL_LEN] = {'\0'};
  char target[MAX_LABEL_LEN] = {'\0'};
  bool directed = false;

  *num_edges = json_object_array_length(json_object_get(edges_json_object));
  hashtable_allocate(*num_edges, edges);

  for (i = 0; i < *num_edges; i++) {
    obj = json_object_array_get_idx(edges_json_object, i);
    json_object_object_foreach(obj, key, val) {
      if (!strcmp(key, "label")) 
        strcpy(label, json_object_to_json_string_ext(val, JSON_C_TO_STRING_PLAIN));
      
      if (!strcmp(key, "source"))
        strcpy(source, json_object_to_json_string_ext(val, JSON_C_TO_STRING_PLAIN));

      if (!strcmp(key, "target"))
        strcpy(target, json_object_to_json_string_ext(val, JSON_C_TO_STRING_PLAIN));

      if (!strcmp(key, "directed"))
        directed = json_object_get_boolean(val);
    }

    if (!strlen(source) || !strlen(target)) {
      fprintf(stderr, "undefined vertices for edge %s\n", label);
      return 1;      
    } 

    e = malloc(sizeof(struct edge));

    if (!strlen(label))
      snprintf(label, MAX_LABEL_LEN, "%d", i);
    e->name = malloc(strlen(label)+1);
    strncpy(e->name, label, strlen(label));

    if (hashtable_lookup(source, vertices, &vertex)) {
      fprintf(stderr, "vertex %s not found in vertices hashtable\n", source);
      return 1;
    } 
    e->x = vertex->value;
    update_vertex_edge_list(vertex, e);

    if (hashtable_lookup(target, vertices, &vertex)) {
      fprintf(stderr, "vertex %s not found in vertices hashtable\n", target);
      return 1;
    } 
    e->y = vertex->value;
    update_vertex_edge_list(vertex, e);

    e->directed = directed;

    hashtable_insert(e->name, e, *edges, &new_edge);

    memset(label,0,MAX_LABEL_LEN);
    memset(source,0,MAX_LABEL_LEN);
    memset(target,0,MAX_LABEL_LEN);
  }

  return 0;
}

int
load_graph(char *name, struct graph **g) {
  char *file_str;
  int num_vertices, num_edges;
  struct json_tokener *tokener;
  struct json_object *obj;
  struct json_object *tmp_obj;

  struct hashtable *vertices;
  struct hashtable *edges;


  if (name==NULL) {
    return load_graph(DEFAULT_GRAPH, g);
  }

  if (read_file_into_string(name, &file_str)) {
    fprintf(stderr, "failed to load %s\n", name);
    return 1;
  }

  tokener = json_tokener_new();
  obj = json_tokener_parse_ex(tokener, file_str, strlen(file_str));

  if (tokener->err != json_tokener_success)
    fprintf(stderr, "Failed to laod %s: %s\n", name,
            json_tokener_error_desc(tokener->err));
  
  json_object_object_foreach(obj, key, val) {
    tmp_obj = json_object_get(val);
    json_object_object_foreach(tmp_obj, key, val) {
      if (!strcmp(key, "nodes"))
        load_vertices(json_object_get(val), &vertices, &num_vertices);
      else
        if (!strcmp(key, "edges"))
          load_edges(json_object_get(val), &edges, &num_edges, vertices);
    }
  }

  /* *g = malloc(sizeof(struct graph)); */
  /* (*g)->num_vertices = num_vertices; */
  /* (*g)->num_edges = num_edges; */
  /* (*g)->vertices = vertices; */
  /* (*g)->edges = edges; */
  
  json_tokener_free(tokener);
  free(file_str);
  return 0;
}

struct edge_list*
create_edge_list(struct edge* edges, int* edge_indices, int size) {
  int index = 0;
  struct edge_list* elist;
  struct edge_list* parent = NULL;
  struct edge_list* elist_head;

  while ( index < size )
    {
      elist = malloc(sizeof(struct edge_list));
      elist->edge = &edges[edge_indices[index]];
      elist->next = NULL;

      if ( parent != NULL )
        parent->next = elist;

      if ( !index )
        elist_head = elist;
      parent = elist;

      index += 1;
    }

  return elist_head;
}

int
free_edge_list(struct edge_list* elist) {
  struct edge_list* temp;

  if ( elist == NULL )
    {
      printf("List is empty.\n");
    }

  while ( elist != NULL ) {
    temp = elist;
    elist = elist->next;
    free(temp);
  } 

  return 0;
}

int
free_vertices(struct vertex *vertices, int size) {
  int i;

  for ( i = 0; i < size; i++ )
    {
      free_edge_list(vertices[i].edges);
    }

  free(vertices);

  return 0;
}

int
free_graph(struct graph *graph) {
  free_vertices(graph->vertices, graph->num_vertices);
  free(graph->edges);
  free(graph);
  return 0;           
}

int
list_vertices(struct vertex* vertices, int size) {
  int i;
  struct edge_list* elist;
  for ( i = 0; i < size; i++ )
    {
      printf("%s -> ", vertices[i].name);
      
      elist = vertices[i].edges;
      
      while ( elist != NULL )
        {
          if ( elist->next != NULL )
            printf("%s, ", elist->edge->name);
          else
            printf("%s", elist->edge->name);
          elist = elist->next;
        }
      printf("\n");
    }

  return 0;
}

int
list_edge_list(struct edge_list *elist) {
  struct edge_list *temp = elist;

  if ( temp == NULL )
    {
      printf("List is empty.\n");
    }

  while ( temp != NULL ) {
    if ( temp == elist )
      printf("%s", temp->edge->name);
    else
      printf(" -> %s", temp->edge->name);
    temp = temp->next;
  }
  printf("\n");
  return 0;
}

int
dummy_graph(struct graph **g) {
  int num_vertices = 6;
  int num_edges = 8;

  *g = malloc(sizeof(struct graph));
  struct vertex *vertices = malloc(num_vertices * sizeof(struct vertex));
  struct edge *edges = malloc(num_edges * sizeof(struct edge));
  (*g)->vertices = vertices;
  (*g)->edges = edges;
  (*g)->num_vertices = num_vertices;
  (*g)->num_edges = num_edges;

  edges[0].name = "e0";
  edges[0].x = &vertices[0];
  edges[0].y = &vertices[1];
  edges[0].flag = false;

  edges[1].name = "e1";
  edges[1].x = &vertices[1];
  edges[1].y = &vertices[2];
  edges[1].flag = false;

  edges[2].name = "e2";
  edges[2].x = &vertices[0];
  edges[2].y = &vertices[3];
  edges[2].flag = false;

  edges[3].name = "e3";
  edges[3].x = &vertices[0];
  edges[3].y = &vertices[4];
  edges[3].flag = false;

  edges[4].name = "e4";
  edges[4].x = &vertices[1];
  edges[4].y = &vertices[4];
  edges[4].flag = false;

  edges[5].name = "e5";
  edges[5].x = &vertices[5];
  edges[5].y = &vertices[2];
  edges[5].flag = false;

  edges[6].name = "e6";
  edges[6].x = &vertices[3];
  edges[6].y = &vertices[4];
  edges[6].flag = false;

  edges[7].name = "e7";
  edges[7].x = &vertices[4];
  edges[7].y = &vertices[5];
  edges[7].flag = false;

  vertices[0].name = "0";
  vertices[0].edges = create_edge_list(edges, (int[]){0, 2, 3}, 3);
  vertices[0].edge_list_ptr = vertices[0].edges;

  vertices[1].name = "1";
  vertices[1].edges = create_edge_list(edges, (int[]){0, 1, 4}, 3);
  vertices[1].edge_list_ptr = vertices[1].edges;

  vertices[2].name = "2";
  vertices[2].edges = create_edge_list(edges, (int[]){1, 5}, 2);
  vertices[2].edge_list_ptr = vertices[2].edges;

  vertices[3].name = "3";
  vertices[3].edges = create_edge_list(edges, (int[]){2, 6}, 2);
  vertices[3].edge_list_ptr = vertices[3].edges;

  vertices[4].name = "4";
  vertices[4].edges = create_edge_list(edges, (int[]){6, 3, 4, 7}, 4);
  vertices[4].edge_list_ptr = vertices[4].edges;

  vertices[5].name = "5";
  vertices[5].edges = create_edge_list(edges, (int[]){7, 5}, 2);
  vertices[5].edge_list_ptr = vertices[5].edges;

  return 0;
}

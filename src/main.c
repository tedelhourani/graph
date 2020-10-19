#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "graph.h"
#include "util.h"
#include "path.h"

static int verbose;
static char *algorithm_name;
static char *input;
static char *output;
struct hashtable *algorithms;
struct hashtable_slot *slot;

int
init() {
  hashtable_allocate(HASHTABLE_MAX_SIZE, &algorithms);
  ADD_ALGORITHM("maximal-path", maximal_path);
  return 0;
}

int
parse_command(int argc, char *argv[]) {
  int c;

  if ( argc < 2 ) {
    fprintf(stderr, "algorithm option required\n");
    return 1;
  }

  while (1)
    {
      static struct option long_options[] =
        {
          /* These options set a flag. */
          {"verbose", no_argument, &verbose, 1},
          /* These options don’t set a flag.
             We distinguish them by their indices. */
          {"algorithm",  required_argument, 0, 'a'},
          {"input",      optional_argument, 0, 'i'},
          {"output",     optional_argument, 0, 'o'},
          {0, 0, 0, 0}
        };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long (argc, argv, "a:i::o::",
                       long_options, &option_index);

      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c)
        {
        case 0:
          break;

        case 'a':
          algorithm_name = optarg;
          break;

        case 'i':
          input = optarg;
          break;

        case 'o':
          output = optarg;
          break;

        case '?':
          /* getopt_long already printed an error message. */
          break;

        default:
          abort ();
        }
    }

  /* Instead of reporting ‘--verbose’
     and ‘--brief’ as they are encountered,
     we report the final status resulting from them. */
  if (verbose)
    puts ("verbose flag is set");

  /* Print any remaining command line arguments (not options). */
  if (optind < argc)
    {
      printf ("non-option ARGV-elements: ");
      while (optind < argc)
        printf ("%s ", argv[optind++]);
      putchar ('\n');
    }

  return 0;
}

int
main(int argc, char *argv[]) {
  struct graph *g;
  struct edge_list *path = NULL;
  struct hashtable_slot *algorithm;

  init();

  if (parse_command(argc, argv))
    exit(1);

  if (hashtable_lookup(algorithm_name, algorithms, &algorithm)) {
    fprintf(stderr, "algorithm %s not found\n", algorithm_name);
    exit(1);
  }
  else {
    load_graph(input, &g);
    /* (algorithm->f)(g, &g->vertices[2], &path); */
    /* list_edge_list(path); */
    /* free_graph(g); */
  }

  /* list_vertices(g->vertices, g->num_vertices); */
  return 0;
}

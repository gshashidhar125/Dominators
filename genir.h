#include <stdio.h>

#define MAX_DEPTH 2
#define NEST_SIZE_MAX 5  // A random size b/w MAX and MIN is picked for the size of nested statements
#define NEST_SIZE_MIN 2
#define MAXIMUM_VERTICES 100

#define NONE -1
#define UNVISITED 0

void genir_random ( int current_depth, int required_generations);

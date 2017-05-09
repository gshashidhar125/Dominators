#include <stdio.h>
#include <stdlib.h>
#include "genir.h"
#include <time.h>

int cfg[MAXIMUM_VERTICES][MAXIMUM_VERTICES] = { };
int semi[MAXIMUM_VERTICES];
int idom[MAXIMUM_VERTICES];
int label_count = 0;

void print_dot_input_cfg ( const char *name) 
{
	int i, j;
	FILE *fp;
	fp = fopen( name, "w" );
	printf("\nNumber of Basic Blocks produced is = %d\n", label_count);
	fprintf (fp, "digraph cfg {\n");
	for (i = 0; i < label_count; i++)
		for (j = 0; j < label_count; j++)
			if (cfg[i][j] == 1)
				fprintf (fp, "%d -> %d\n", i, j);
				
	fprintf (fp, "}\n");
}
void print_dot_input_matrix ( int x[MAXIMUM_VERTICES], const char *name) 
{
	int i, j;
	FILE *fp;
	fp = fopen(name, "w");
	fprintf (fp, "digraph cfg {\n");
	for (j = 0; j < label_count; j++)
		if (x[j] > -1)
			fprintf (fp, "%d -> %d\n", x[j], j);
	
	fprintf (fp, "}\n");
}


int main (int argc, char *argv[]) {
		
	int i, j;
	if (argc != 2) {
		printf ("Usage: %s number_of_statements (per nesting).\n", argv[0]);
		exit (1);
	}
	
	srand ((int)time (NULL));
	genir_random ( 0, atoi (argv[1]));
	
	label_count++;
	for(i = 0 ; i < label_count; i++)
		printf("%d\t", i);
	printf("\n");	
	for(i = 0 ; i < label_count; i++)
		printf("--------");
	printf("\n");	
	for(i = 0 ; i< label_count; i++)
	{
		printf("%d |", i);
		for(j = 0 ; j< label_count; j++)
			printf("%d\t", cfg[i][j]);
		printf("\n");
	}
	print_dot_input_cfg("cfg.dot");
	
	i = build_dominator_tree();
	
	print_dot_input_matrix( semi, "semi.dot");
	print_dot_input_matrix( idom, "domi.dot");
	
	if (i == 0)
		printf("\nSUCCESS\n");
	else
	    printf("\nFAILED\n");	
	return 0;
}

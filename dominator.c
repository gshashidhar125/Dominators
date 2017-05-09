#include <stdio.h>
#include <stdlib.h>
#include "genir.h"

/* The lengauer-Tarjan Algorithm, as described in the book by Andrew W Appel (compiler implementation */

struct bucket_name
{
	int info;
	struct bucket_name * next;
};
struct bucket_name * bucket[(MAXIMUM_VERTICES)];

extern int cfg[MAXIMUM_VERTICES][MAXIMUM_VERTICES];
extern int semi[MAXIMUM_VERTICES];
extern int idom[MAXIMUM_VERTICES];
extern int label_count;

int dfnum[MAXIMUM_VERTICES] = {},vertex[MAXIMUM_VERTICES] = {},parent[MAXIMUM_VERTICES] = {};
int ancestor[MAXIMUM_VERTICES], samedom[MAXIMUM_VERTICES];

int N;

static void dfs (int p, int n) 
{
	
	int i;
	if (dfnum[n] == UNVISITED ) 
	{
		dfnum[n] = N;
		vertex[N-1] = n;
		parent[n] = p;
		N++;
		for (i = 0;i < label_count;i++) 		
			if (cfg[n][i] == 1)
				dfs(n, i);
	}	
}

int ancestorWithLowestSemidominator (int v) 
{
	int u = v;
	while (ancestor [v] != NONE) 
	{
		if (dfnum[semi[v]] < dfnum[semi[u]])
			u = v;
		v = ancestor[v];		
	}
	return u;
}

void link (int p, int n)
{
	ancestor[n] = p;
}

void bucket_insert(int s, int n)
{
	struct bucket_name * p, * q, *r;
	r = malloc(sizeof(struct bucket_name));
	r->info = n;
	r->next = NULL;
	p = q = bucket[s];
	while (p != NULL)
	{
		q = p;
		p = p->next;
	}	
	if (p == q)
		bucket[s] = r;
	else
	 	q->next = r;
}
/* node 0 is assumed to be the root */
/* for any other root, modify the call to dfs, 
and the last statement to set dominator of the root */

int build_dominator_tree () 
{
	int i,j,n,p,r,s,y;
	struct bucket_name * b;
	N = 1;
	for (i = 0; i < label_count; i++)
	{
		semi[i] = ancestor[i] = idom[i] = samedom[i] = -1;
	}
	dfs(NONE,0);
	for (i = N-1; i > 0; i--)
	{
		n = vertex[i];
		p = parent[n];
		s = p;
		for (j = 0;(j < label_count); j++)
		{
			if (cfg[j][n] == 1)
			{
				if (dfnum[j] <= dfnum[n])
					r = j;
				else
					r = semi[ancestorWithLowestSemidominator(j)];
				if (dfnum[r] < dfnum[s])
					s = r;		
			}		
		}
		semi[n] = s;
		bucket_insert(s,n);
		link(p,n);		
		b = bucket[p];
		for (; b != NULL; b = b->next)
		{
			j = b->info;
			y = ancestorWithLowestSemidominator(j);
			if (semi[y] == semi[j])
				idom[j] = p;
			else
			 	samedom[j] = y;			 					 	
		}
		bucket[p] = NULL;
	}	
	for (i = 1; i < N; i++)
	{
		n = vertex[i];
		if (samedom[n] != NONE)
			idom[n] = idom[samedom[n]];
	}
	return 0; 
	/* this function, as of now, just sets the immediate dominator for each node 'i' as 'i' itself. */
}
/* ----------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "genir.h"
#include <unistd.h>
#include <time.h>


/* We generate the following kinds of code.

1. x = y 'op' z
2. x = 'op' y		('op' is a unary operator)
3. x = y
4. goto L - As of now, used only inside if x 'relop' y (for if else)
5. if x 'relop' y goto L

These still need to be added.
6. x = y[i] and x[i] = y
7. x = &y, x = *y and *x = y

NOTE: as of now, only integer datatypes are used. 
*/


extern int cfg[MAXIMUM_VERTICES][MAXIMUM_VERTICES];
extern int label_count;
static int count = 0, leader = 0, status = 0; /*These are control flags */


/*Function that write the output of the adjacency matrix to "cfg.dot" for viewing it in 'dotty'*/


void genir_random ( int current_depth, int required_generations) {
	
	int generations, random_value;
	
	for (generations = 0; generations < required_generations; generations++) {
		
		random_value = random () % 12;
		switch (random_value) {
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
				{
				/* x = y 'op' z */
				int x, y, z; char op;
				x = random() % MAXIMUM_VERTICES;
				/* Decide if y is a constant of a variable.
				1/3rd probability for a constant */
				if ((random() % 3) == 0)
					y = -1;
				else
					y = random() % MAXIMUM_VERTICES;
				/* similarly for z */
				if ((random() % 3) == 0)
					z = -1;
				else
					z = random() % MAXIMUM_VERTICES;
				/* Now decide on the operator */
				switch (random() % 4) {
					case 0:
						op = '+';
						break;
					case 1:
						op = '-';
						break;
					case 2:
						op = '*';
						break;
					case 3:
						op = '/';
						break;
				}
				if (y == -1 && z == -1)
				printf ("\tt%d = %d %c %d\n", x, ((int)random()%100),op,((int)random()%100));
				else if (y == -1 && z >= 0)
				printf ("\tt%d = %d %c t%d\n", x, ((int)random()%100), op, z);
				else if (y >= 0 && z == -1)
				printf ("\tt%d = t%d %c %d\n", x, y, op, ((int)random()%100));
				else if (y >= 0 && z >= 0)
				printf ("\tt%d = t%d %c t%d\n", x, y, op, z);				
				status = 1;/*this flag is set to know that we got the leader for the block 										and is checked in loooping statements*/
				break;
			}
			case 5: {
				/* x = -y (currently only unary minus) */
				int x, y;
				x = random() % MAXIMUM_VERTICES;
				/* decide if y is a variable or a constant */
				y = random() % MAXIMUM_VERTICES;
				if (random() % 3 == 0) // one of three chances is a constant
					y = -1;
				if (y == -1)
					printf ("\tt%d = -%d\n", x, ((int)random()%100));
				else
					printf ("\tt%d = -t%d\n", x, y);
				status = 1;/*this flag is set to know that we got the leader for the block 										and is checked in loooping statements*/
				break;
			}
			case 6:
			case 7: {
				/* Copy statements */
				int x, y;
				x = random() % MAXIMUM_VERTICES;
				y = random() % MAXIMUM_VERTICES;
				if ((random() % 4) == 0) // one of four chances is constant
					y = -1;
				if (y == -1)
					printf ("\tt%d = %d\n", x, ((int)random()%100));
				else
					printf ("\tt%d = t%d\n", x, y);
				status = 1;/*this flag is set to know that we got the leader for the block 										and is checked in loooping statements*/
				break;
			}
			case 8: 
			case 9:
				{
				/* generate if and if else statements */
				int ifbegin, ifend, x, y, nest_size, elsebegin;
				char relop[3];
				
				if (current_depth >= MAX_DEPTH) break;
				/* we shouldn't exceed the maximum nesting depth */
						
				ifbegin = ++label_count;
				ifend = ++label_count;				
				
				x = random() % MAXIMUM_VERTICES;
				y = random() % MAXIMUM_VERTICES;
				
				if (random() % 3 == 0) // one of three chances, x is a constant.
					x = -1;
				if (random() % 3 == 0) // one of three chances, y is a constant.
					y = -1;
				/* The case where both x and y are constants, evaluates
				to a constant expression. This might help in dead code elimination
				and other simplification. */

				switch (random() % 6) {
					case 0:
						strcpy (relop, "==");
						break;
					case 1:
						strcpy (relop, "!=");
						break;
					case 2:
						strcpy (relop, "<");
						break;
					case 3:
						strcpy (relop, "<=");
						break;
					case 4:
						strcpy (relop, ">");
						break;
					case 5:
						strcpy (relop, ">=");
						break;
				}
				if (x == -1 && y == -1)
					printf ("\tif %d %s %d goto ifbegin%d\n", 
						 ((int)random()%100), relop, ((int)random()%100), ifbegin);
				else if (x == -1 &&  y >= 0)
					printf ("\tif %d %s t%d goto ifbegin%d\n",
						 ((int)random()%100), relop, y, ifbegin);
				else if (x >= 0 && y == -1)
					printf ("\tif t%d %s %d goto ifbegin%d\n",
						 x, relop, ((int)random()%100), ifbegin);
				else if (x >= 0 && y >= 0)
					printf ("\tif t%d %s t%d goto ifbegin%d\n",
						 x, relop, y, ifbegin);
				
				/*I have used the values of 'ifbegin','ifend' for the basic block node 																				values*/		
				/* decide whether if or if else */

				if ((random() % 3) % 2 == 1) { /* equal chances for both */
					/*
						if x 'relop' y goto ifbegin
						goto ifend
					ifbegin:
						....
						....
					ifend:
						...
					*/
					
					cfg[count][ifbegin] = 1;  // These statements set the cfg matrix for the 
					cfg[count][ifend] = 1;	  // 'if' statement and set the flag 'leader'	
					leader = 0;               // to indicate that we got a leader for the 
					count = ifbegin;          // given basic block
					printf ("\tgoto ifend%d\n", ifend);
					printf ("ifbegin%d:\n", ifbegin);
					nest_size= (random()%(NEST_SIZE_MAX-NEST_SIZE_MIN))+NEST_SIZE_MIN;
					
					genir_random ( current_depth+1, nest_size);
					printf ("ifend%d:\n", ifend);
					
					cfg[count][ifend] = 1;    // This set the cfg matrix for the false 
					count = ifend;            // condition of the previous 'if' statement
					
				}
				else {
					/*
						if x 'relop' y goto ifbegin
						goto elsebegin
					ifbegin:
						....
						....
						goto ifend
					elsebegin:
						....
						....
					ifend:
						...
					*/
					/*I have used the values of 'ifbegin','ifend','elsebegin' for the basic 																block node values*/		
					elsebegin = ++label_count;
					cfg[count][ifbegin] = 1;  // These statements set the cfg matrix for the 
					cfg[count][elsebegin] = 1;// 'if' and else statement of the 'if else' 
					count = ifbegin;          // statement and set the flag 'leader' to 
					leader=1;                 // indicate that we got a leader for the given 
										      // basic block
					printf ("\tgoto elsebegin%d\n", elsebegin);
					printf ("ifbegin%d:\n", ifbegin);
					nest_size = (random()%(NEST_SIZE_MAX-NEST_SIZE_MIN))+NEST_SIZE_MIN;
					genir_random ( current_depth+1, nest_size);
					
					cfg[count][ifend] = 1; // For the control flow outside the 'if' clause
										   // of the 'if else' block
										
					printf ("\tgoto ifend%d\n", ifend);									
					printf ("elsebegin%d:\n", elsebegin);
					nest_size = (random()%(NEST_SIZE_MAX-NEST_SIZE_MIN))+NEST_SIZE_MIN;
					
					count = elsebegin;
					leader = 1;
					
					genir_random ( current_depth+1, nest_size);
					
					cfg[count][ifend] = 1; // For the control flow outside the 'else' clause
					count = ifend;         // of the'if else' block	
					leader = 1;
					
					printf ("ifend%d:\n", ifend);				
				}
				break;
			}
			case 10:
			case 11: {
				/* Lets have a while loop now */
				int loopbegin, loopend, loopbody, x, y;
				char relop[3];
				int nest_size;
				
				if (current_depth >= MAX_DEPTH) break;
				/* we shouldn't exceed the maximum nesting depth */
				
				if (label_count == 0 && status == 0)
					loopbegin = 0;	
				else	
					loopbegin = ++label_count;
					
				loopbody = ++label_count;
				loopend = ++label_count;
				
				nest_size = (random()%(NEST_SIZE_MAX-NEST_SIZE_MIN))+NEST_SIZE_MIN;
				
				x = random() % MAXIMUM_VERTICES;
				y = random() % MAXIMUM_VERTICES;
				
				if (random() % 3 == 0) // one of three chances, x is a constant.
					x = -1;
				if (random() % 3 == 0) // one of three chances, y is a constant.
					y = -1;
				/* The case where both x and y are constants, evaluates
				to a constant expression. This might help in dead code elimination
				and other simplification. */

				switch (random() % 6) {
					case 0:
						strcpy (relop, "==");
						break;
					case 1:
						strcpy (relop, "!=");
						break;
					case 2:
						strcpy (relop, "<");
						break;
					case 3:
						strcpy (relop, "<=");
						break;
					case 4:
						strcpy (relop, ">");
						break;
					case 5:
						strcpy (relop, ">=");
						break;
				}
				
				/* 
				loopbegin: 
					if x 'relop' y goto loopbody
					goto loopend 
				loopbody:
					.....
					.....
					goto loopbegin
				loopend:
				*/
				
				/* I have used the values the values of the variables 'loopbegin', 					'loopbody','loopend' for numbering the basic blocks created during loop 				execution*/
				if(count != loopbegin)			
					cfg[count][loopbegin] = 1;  
										    	// These statements sets the cfg matrix from 
				cfg[loopbegin][loopbody] = 1;   // the condition block to the loop body
				count = loopbody;				// and sets the leader flag to tell that
				leader = 1;						// we already have a leader
				
				printf ("loopbegin%d:\n", loopbegin);
				if (x == -1 && y == -1)
					printf ("\tif %d %s %d goto loopbody%d\n", 
						 ((int)random()%100), relop, ((int)random()%100), loopbody);
				else if (x == -1 &&  y >= 0)
					printf ("\tif %d %s t%d goto loopbody%d\n",
						((int)random()%100), relop, y, loopbody);
				else if (x >= 0 && y == -1)
					printf ("\tif t%d %s %d goto loopbody%d\n",
						x, relop, ((int)random()%100), loopbody);
				else if (x >= 0 && y >= 0)
					printf ("\tif t%d %s t%d goto loopbody%d\n",
						x, relop, y, loopbody);
				
				printf ("\tgoto loopend%d\n", loopend);
				printf ("loopbody%d:\n", loopbody);
				
				genir_random ( current_depth+1, nest_size);
				printf ("\tgoto loopbegin%d\n", loopbegin);
				
				cfg[count][loopbegin] = 1;  // Sets control back to the loopbegin for looping
				
				printf ("loopend%d:\n", loopend);
				
				cfg[loopbegin][loopend] = 1;// Sets control from the loop condition
				count = loopend;			// to the loop end for termination of the loop
				leader = 1;
				break;
			}
		}
	}
}
/*
int main (int argc, char *argv[]) {
	int i,j,basic_blocks;
	if (argc != 2) {
		printf ("Usage: %s number_of_statements (per nesting).\n", argv[0]);
		exit (1);
	}
	
	genir_random (0,atoi (argv[1]));
	
	label_count++;
	for(i=0 ; i< label_count;i++)
		printf("%d\t",i);
	printf("\n");	
	for(i=0 ; i< label_count;i++)
		printf("--------");
	printf("\n");	
	for(i=0 ; i< label_count;i++)
	{
		printf("%d |",i);
		for(j=0 ; j< label_count;j++)
			printf("%d\t",cfg[i][j]);
		printf("\n");
	}
	print_dot_input("cfg");
	return 0;
}*/

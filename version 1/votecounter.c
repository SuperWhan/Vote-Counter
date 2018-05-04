/*login: huxxx990, shixx566
* date: 02/21/18
* name: Hanyu Hu, Xicun Shi
* id: 5272396, 5090878 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "makeargv.h"

#define MAX_NODES 100

char** candidates;
int num_candidates;
int length;


//Function signatures



/**Function : parseInputLine
 * Arguments: 's' - Line to be parsed
 * 			  'n' - Pointer to Nodes to be allocated by parsing
 * Output: Number of Region Nodes allocated
 * About parseInputLine: parseInputLine is supposed to
 * 1) Split the Input file [Hint: Use makeargv(..)]
 * 2) Recognize the line containing information of
 * candidates(You can assume this will always be the first line containing data).
 * You may want to store the candidate's information
 * 3) Recognize the line containing "All Nodes"
 * (You can assume this will always be the second line containing data)
 * 4) All the other lines containing data, will show how to connect the nodes together
 * You can choose to do this by having a pointer to other nodes, or in a list etc-
 * */
 int parseInputLine(char *s, node_t *n)
 {
 	char **out_string = (char **)malloc(1024 * sizeof(char*));
	int dec = makeargv(s, ":", &out_string);
 	if (dec == 1)
 	{
 		// if this is the second line
		length = makeargv(s, " ", &out_string);
 		if(strncmp(out_string[0], "Who_Won", (int)strlen("Who_Won"))==0)
 		{
 			for (int i=0; i < length ; i++)
 			{
 			 strcpy(n[i].name, trimwhitespace(out_string[i]));
			 n[i].id = i+1;

 		 } // Recognize the line containing "All Nodes"
		 return 0;
 		}
 		// if this is the first line
 		else
 		{
 			num_candidates = atoi(out_string[0]); // number of candidates
 			candidates = (char **) malloc (num_candidates * sizeof(char *));
 			candidates[0] = NULL;
 			for (int i = 1; i<length; i++)
 			{
 				candidates[i] = out_string[i];
 			}
 			return 0;
 		}
 	}
 	else // if it is not the first or second line
 	{
	 		char **out_string2 = (char **)malloc(1024 * sizeof(char*));
	    int child_len = makeargv(out_string[1], " ", &out_string2);
	    for(int i=0; i<length; i++)
	    {
	 	 	if (strncmp(n[i].name, out_string[0], ((int)strlen(out_string[0])-1))==0 )
	  	 {
	    		int child_count = 0;
					n[i].num_children = child_len;
	   		  for(int a=0; a < child_len; a++)
	   		 {
	  			 for(int j=0; j< length; j++)
	  			 {
	 				 if(strncmp(n[j].name, out_string2[a], ((int)strlen(out_string2[a])))==0)
	 				 	{
	  					 n[i].children[child_count] = n[j].id;
	  					 child_count++;
							}
	  			 }
	   		 }
	  	 }
     }
	 return child_len;
 	}
 	return 0;
 }

/**Function : parseInput
 * Arguments: 'filename' - name of the input file
 * 			  'n' - Pointer to Nodes to be allocated by parsing
 * Output: Number of Total Allocated Nodes
 * About parseInput: parseInput is supposed to
 * 1) Open the Input File [There is a utility function provided in utility handbook]
 * 2) Read it line by line : Ignore the empty lines [There is a utility function provided in utility handbook]
 * 3) Call parseInputLine(..) on each one of these lines
 ..After all lines are parsed(and the DAG created)
 4) Assign node->"prog" ie, the commands that each of the nodes has to execute
 For Leaf Nodes: ./leafcounter <arguments> is the command to be executed.
 Please refer to the utility handbook for more details.
 For Non-Leaf Nodes, that are not the root node(ie, the node which declares the winner):
 ./aggregate_votes <arguments> is the application to be executed. [Refer utility handbook]
 For the Node which declares the winner:
 This gets run only once, after all other nodes are done executing
 It uses: ./find_winner <arguments> [Refer utility handbook]
 */
int parseInput(char *filename, node_t *n)
{
	// number of total allocated nodes
	int totalNodes = 0;
	// open the input file
	FILE* fd = file_open(filename);
	// call parseInputLine on each one of these lines
	char* buffer = (char*)malloc(sizeof(char)*1024*1024);

	while((buffer = read_line(buffer,fd)) != NULL) {
	totalNodes += parseInputLine(buffer, n);
	}
  free(buffer);
  for(int i = 0; i<length; i++) {
		if(strncmp(n[i].name, "Who_Won", strlen(n[i].name)) == 0) {
		strncpy(n[i].prog, "./find_winner", 1023);
		n[i].prog[1023] = 0;
  } else if(n[i].num_children == 0) {
			strncpy(n[i].prog, "./leafcounter", 1023);
			n[i].prog[1023] = 0;
		} else {
			strncpy(n[i].prog, "./aggregate_votes", 1023);
			n[i].prog[1023] = 0;
		}
	}
	return totalNodes;
}


/**Function : execNodes
 * Arguments: 'n' - Pointer to Nodes to be allocated by parsing
 * About execNodes: parseInputLine is supposed to
 * If the node passed has children, fork and execute them first
 * Please note that processes which are independent of each other
 * can and should be running in a parallel fashion
 * */
void execNodes(node_t *n)
{

	if(n->num_children == 0)
	{
		char *buffer = (char*)malloc(sizeof(int)*4+1);
		char fileName[] = "output_";
		strcat(fileName, n->name);
		strcat(fileName, ".txt");
		int fd = open(fileName, O_RDWR|O_CREAT);
		// creat output file name
		char* args[5 + num_candidates];
		args[0] = "./leafcounter";
		char* temp = (char*)malloc(strlen(n->name)+1);
		strcpy(temp, n->name);
		sprintf(buffer, "%d", num_candidates);
		args[1] = temp;
		args[2] = fileName;
		args[3] = buffer;
		for(int i = 4; candidates[i-4] != NULL; i++)
		{
			args[i] = candidates[i-4];
		}
			args[5 + num_candidates -1] = NULL;
			printf("%s\n", args[3]);
			execv("./leafcounter", args);
			return;
	  } else {
			pid_t pid;
			char* child_file[n->num_children];
			for(int i = 0; i < n->num_children; i ++) {
				node_t* child_node = findNodeByID(n, n->children[i]);
				pid = fork();
				if (pid == 0) {
					execNodes(child_node);
				} else {
					wait(NULL);
					char fileName[] = "output_";
					strcat(fileName, child_node->name);
					char* temp = (char*)malloc((strlen(fileName)+1)*sizeof(char));
					strcpy(temp, fileName);
					child_file[i] = temp;
					continue;
				 }
			}
			if (pid != 0) {
						 wait(NULL);
						 char *buffer1 = (char*)malloc(sizeof(int)*4+1);
						 char *buffer2 = (char*)malloc(sizeof(int)*4+1);
						 char* args[5 + n->num_children + num_candidates];
						 sprintf(buffer1, "%d", n->num_children);
						 sprintf(buffer2, "%d", num_candidates);
						 args[0] = "./aggregate_votes";
						 args[1] = buffer1;

						 int i;
						 for (i = 2; i < n->num_children; i ++) {
							 args[i] = child_file[i-2];
						 }
						 // insert the output file name
						 args[i++] = "output_";
						 strcat(args[i], n->name);
						 strcpy(args[i++],buffer2);
						 int sub = i;
						 for(; candidates[i-sub] != NULL; i ++) {
							 args[i] = candidates[i-sub];
						 }
						 args[i] = NULL;
						 if (strncmp(n->name, "Who_Won", strlen(n->name)) != 0) {
							 execv("./aggregate_votes", args);
						 } else {
							 args[0] = "./find_winner";
							 execv("./find_winner", args);
						 }
						 return;
					 }
		}
}


int main(int argc, char **argv){

	//Allocate space for MAX_NODES to node pointer
	struct node* mainnodes=(struct node*)malloc(sizeof(struct node)*MAX_NODES);

	if (argc != 2){
		printf("Usage: %s Program\n", argv[0]);
		return -1;
	}
	//call parseInput
	int num = parseInput(argv[1], mainnodes);

  execNodes(mainnodes);

	// printgraph(mainnodes, num);

	//Call execNodes on the root node


	return 0;
}

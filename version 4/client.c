#define _BSD_SOURCE
#define NUM_ARGS 3

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "makeargv.h"

#define SERVER_PORT 2396
#define MAX_CONNECTIONS 5
#define CANDIDATE_NAME 100
#define REGION_NAME 15
#define MAX_CANDIDATES 100
#define MAX_REGIONS 100
#define MESSAGE_SIZE 256

struct candidate {
  char *name;
  int vote;
};

struct input {
  char *request_name;
  char *region_name;
  char *vote_file;
};

struct input* request;
// struct candidate* candi;
void * read_input(char * filename);
char* make_request(struct input* req);
char * count_vote(char * filename);

char* temp;
int list_counter;

// int candi_counter = 0;



int main(int argc, char** argv) {
  /*
  usage: ./client <REQ FILE> <Server IP> <Server Port>
  */

  if (argc < NUM_ARGS + 1) {

		printf("Wrong number of args, expected %d, given %d\n", NUM_ARGS, argc - 1);
		exit(1);
	}

  char* input = argv[1];
  char* input2 = argv[1];
  char** path;
  temp = malloc(1024);
  int a = makeargv(input, "/", &path);
  // printf("%s\n", path[a-1]);

  for (int i = 0 ; i < (strlen(input2)-strlen(path[a-1])) ; i++) {
    temp[i] = input2[i];
  }
  // printf("!!!!!!!!!!!!!!!!%s\n", temp);
  read_input(input);
  // for(int i=0; request[i].request_name != NULL; i++) {
  //   printf("1:%s, 2:%s, 3:%s\n", request[i].request_name, request[i].region_name, request[i].vote_file);
  // }



  // Create a TCP socket.
	int sock = socket(AF_INET , SOCK_STREAM , 0);

	// Specify an address to connect to (we use the local host or 'loop-back' address).
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(SERVER_PORT);
	address.sin_addr.s_addr = inet_addr(argv[2]);

	// Connect it.
	if (connect(sock, (struct sockaddr *) &address, sizeof(address)) == 0) {
		printf("Initiated connection with server at %s:%d\n", argv[2], SERVER_PORT);
    for (int i = 0; i < list_counter; i++) {
      char* buffer = make_request(request);
      printf("%s\n", buffer);  
    }

		// Close the socket.
		close(sock);

	} else {

		perror("Connection failed!");
	}
}

char* make_request(struct input* req) {
  char *message = malloc(MESSAGE_SIZE);
  char region_space[15];
  memset(region_space, ' ', 15*sizeof(char));

  char* temp_path = malloc(1024);

  for (int i = 0; req[i].request_name != NULL; i++) {
    char* request = req[i].request_name;
    char* region = req[i].region_name;
    char* vote = req[i].vote_file;

    strcpy(temp_path, temp);
    // printf("1:%s, 2:%s, 3:%s\n", request, region, vote);
    if (strcmp(request, "Return_Winner") == 0) {
      // strcpy(region_space, region);
      // memset(region_space+strlen(region), ' ', 15*sizeof(char)-strlen(region));
      sprintf(message, "RW;%s;%c",region_space,'\0');
    }
    if (strcmp(request, "Count_Votes") == 0) {
      strcpy(region_space, region);
      memset(region_space+strlen(region), ' ', 15*sizeof(char)-strlen(region));
      sprintf(message, "CV;%s;%c", region_space,'\0');
    }
    if (strcmp(request, "Open_Polls") == 0) {
      strcpy(region_space, region);
      memset(region_space+strlen(region), ' ', 15*sizeof(char)-strlen(region));
      sprintf(message, "OP;%s;%c", region_space,'\0');
    }
    if (strcmp(request, "Add_Votes") == 0) {
      strcat(temp_path, vote);

      strcpy(region_space, region);
      memset(region_space+strlen(region), ' ', 15*sizeof(char)-strlen(region));
      sprintf(message, "AV;%s;%c", region_space,'\0');
      strcat(message, count_vote(temp_path));

    }
    if (strcmp(request, "Remove_Votes") == 0) {
      strcat(temp_path, vote);

      strcpy(region_space, region);
      memset(region_space+strlen(region), ' ', 15*sizeof(char)-strlen(region));
      sprintf(message, "RV;%s;%c", region_space,'\0');
      strcat(message, count_vote(temp_path));

    }
    if (strcmp(request, "Close_Polls") == 0) {
      strcpy(region_space, region);
      memset(region_space+strlen(region), ' ', 15*sizeof(char)-strlen(region));
      sprintf(message, "CP;%s;%c", region_space,'\0');
    }
  }
  return message;
}

char * count_vote(char * filename) {
  FILE *vote = fopen(filename, "r");
  if (vote == NULL) {
    perror("Failed to open the input file");
    exit(1);
  }
  char *line;
  char *data = malloc(1024);
  char *temp_s = malloc(1024);
  size_t len = 0;
  ssize_t read;

  struct candidate* candi = (struct candidate*) malloc (sizeof(struct candidate)* MAX_CANDIDATES);
  int list_counter = 0;
  int candi_counter = 0;
  while ((read = getline(&line, &len, vote)) != -1) {
    char** array;
    int n = makeargv(trimwhitespace(line), "\n", &array);
    for(int i=0; i<n; i++) {
      // printf("befoer:%s\n", array[i]);
      int j = 0;
      while (j<= candi_counter) {
        if(j == candi_counter) {
          candi[candi_counter].name = array[i];
          candi[candi_counter].vote = 1;
          candi_counter++;
          break;
        }
        if(strcmp(candi[j].name,array[i]) == 0) {
          candi[j].vote += 1;
          break;
        }
        j++;
      }
    }
    list_counter++;
  }
  for(int i =0; i<candi_counter; i++) {
    sprintf(data,"%s:%d,", candi[i].name, candi[i].vote);
    strcat(temp_s, data);
  }
  temp_s[strlen(temp_s)-1] = 0;
  free(data);
  return temp_s;
}


void * read_input(char * filename) {
  FILE *input = fopen(filename, "r");
  if (input == NULL) {
    perror("Failed to open the input file");
    exit(1);
  }
  char *line;
  size_t len = 0;
  ssize_t read;

  request = (struct input*) malloc (sizeof(struct input) * 100);

  list_counter = 0;
  while ((read = getline(&line, &len, input)) != -1) {
    char** array;
    int n = makeargv(trimwhitespace(line), " ", &array);
    for(int i=0; i<n; i++) {
      // char** request_array;
      // int m = makeargv(trimwhitespace(line), " ", &request_array);
      request[list_counter].request_name = array[0];
      request[list_counter].region_name = array[1];
      request[list_counter].vote_file = array[2];

    }
    list_counter++;
  }
printf("%d\n", list_counter);

}

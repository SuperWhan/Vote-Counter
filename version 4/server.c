#define _BSD_SOURCE
#define NUM_ARGS 2

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

struct dnode // region
{
  char *name;
  // char* path;
  int id;
  int parent;
  int child[10];
  int num_children;
  int poll_flag; // three state: initial unopen, open, close
  struct candidate candi[MAX_CANDIDATES]; // contain the vote data
};

void read_DAG(char * filename);
void * connection_handler(void * socketfd);
// void * read_request(char *message);
char * read_request(char *message);

int * read_children(int * l);


// Global variable
struct candidate* candi[MAX_CANDIDATES];
struct dnode* region;
int child_list[MAX_REGIONS] = {0};
int counter = 0;
int temp_list2[100] = {0};
int temp_list3[100] = {0};
struct dnode* error[MAX_REGIONS];
// struct dnode* root;
char *ip; // Store the ip address of client


int * read_children(int * l) {
  int temp_list[100] = {0};
  int ohyeah = 0;
  int ohno = 0;
  int j = 0;
  for (int i = 0; i < 100; i++) {
    temp_list3[i] = l[i];
  }
  if (temp_list3[0] == 0) {
    for (; j < region[0].num_children; j++) {
      counter++;
      child_list[counter] = region[0].child[j];
      temp_list[j] = region[0].child[j];
      // printf("insert %d\n", region[l[i]].child[j-ohno]);
    }
  }

  else {
    for (int i = 0; temp_list3[i] != 0; i++) {
      ohno = j;
      for (; j < region[l[i]].num_children + ohno; j++) {
        counter++;
        child_list[counter] = region[l[i]].child[j-ohno];
        temp_list[j] = region[l[i]].child[j-ohno];
        // printf("insert %d\n", region[l[i]].child[j-ohno]);
      }
    }
  }

  // printf("%d\n", temp_list[2]);
  for (int i = 0; i < 100; i++) {
    temp_list2[i] = temp_list[i];
  }

  // printf("%d\n", temp_list2[0]);
  while (temp_list[ohyeah] != 0) {
    if (region[temp_list[ohyeah]].num_children != 0) {
      return read_children(temp_list2);
    }
    ohyeah ++;
  }

  return child_list;
}

int main(int argc, char** argv) {
  /*
  usage: ./server <DAG FILE> <Server Port>
  */

  if (argc < NUM_ARGS + 1) {

		printf("Wrong number of args, expected %d, given %d\n", NUM_ARGS, argc - 1);
		exit(1);
	}

  // Create a TCP socket.
  int sock = socket(AF_INET , SOCK_STREAM , 0);

  // Read the DAG file
  char * dag = argv[1];
  read_DAG(dag);
  // Bind it to a local address.
	struct sockaddr_in servAddress;
	servAddress.sin_family = AF_INET;
	servAddress.sin_port = htons(SERVER_PORT);
	servAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sock, (struct sockaddr *) &servAddress, sizeof(servAddress));

  // We must now listen on this port.
	listen(sock, MAX_CONNECTIONS);
  printf("Server listening on port %d\n", SERVER_PORT);

  // A server typically runs infinitely, with some boolean flag to terminate.
	while (1) {

		// Now accept the incoming connections.
		struct sockaddr_in clientAddress;

		socklen_t size = sizeof(struct sockaddr_in);
		int clientfd = accept(sock, (struct sockaddr*) &clientAddress, &size);
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddress.sin_addr), ip, INET_ADDRSTRLEN);
    printf("Connection initiated from client at %s:%d\n", ip, ntohs(clientAddress.sin_port));
    pthread_t pid;
    int *new_sock = malloc(1);
    *new_sock = clientfd;

    if (pthread_create(&pid, NULL, connection_handler, (void*) new_sock) < 0) {
      perror("Could not create thread");
      return 1;
    }
    // Now join the thread , so that we dont terminate before the thread
    // pthread_join(pid, NULL);

	}

	// Close the socket.
	close(sock);
}

void * connection_handler(void * socketfd) {
  // Get the socket descriptor
  int socket = *(int *)socketfd;
  int read_size;
  // Buffer for data.
  char *buffer = malloc(MESSAGE_SIZE);
  char *message;
  // Read from the socket and print the contents.
  if (socket == -1) {
    perror("Failed to accept connection");
    exit(EXIT_FAILURE);
  }

  // Receive a message from client
  while ((read_size = recv(socket, buffer, MESSAGE_SIZE, 0)) > 0) {

    printf("Request received from client at <CLIENT IP>:<CLIENT PORT>,%s\n", buffer);
    // send response to client
    char * response = read_request(buffer);
    printf("Sending response to client at <CLIENT IP>:<CLIENT PORT>,%s\n", response);
    write(socket, response, strlen(buffer));
  }
  if (read_size == 0) {
    puts("Closed connection with client at <CLIENT IP>:<CLIENT PORT>");
    fflush(stdout);
  } else if (read_size == -1) {
    perror("recv failed");
  }

  free(socketfd);
}

char * read_request(char *message) {
  char** array;
  int n = makeargv(trimwhitespace(message), ";", &array);
  char* response = (char*) malloc(MESSAGE_SIZE);
  // printf("%s\n", array[1]);
  char * header = array[0];
  char * region_name = array[1];
  char * vote = array[2];


  // Show the DAG structure
  // int i = 0;
  // printf("%s\n", region[i].name);
  // while(region[i].name != NULL) {
  //   printf("name:%s\n", region[i].name);
  //   printf("parent:%d\n", region[i].parent);
  //   printf("num_children%d\n", region[i].num_children);
  //   printf("id%d\n", region[i].id);
  //   i++;
  // }
  if (strcmp(header, "RW") == 0) { // Retuen winner
    response = "SC;\0";
  } else if (strcmp(header, "CV") == 0) { // Count votes
    response = "SC;\0";
  } else if (strcmp(header, "OP") == 0) { // Open poll
    int check_exit = 0; // False. There is no such region
    for (int i = 0; region[i].name != NULL; i++) {
      if (strcmp(region[i].name, trimwhitespace(region_name)) == 0) {
        // printf("%s, %d\n", region[i].name, region[i].id);
        if (region[i].poll_flag == 1) {
          sprintf(response, "PF;%s:open.%c", region_name, '\0');
        }
        check_exit = 1; // True. Region found
        child_list[0] = region[i].id;
        read_children(child_list);
        // for (int index = 0; index < counter; index++) {
        //   if (strcmp(region[index].name, trimwhitespace(region_name)) == 0) {
        //
        //   }
        // }

        for (int index = 0; index < counter+1; index++) {

          // if (region[child_list[index]].poll_flag == 1) {
          //   sprintf(response, "ERROR;%s:open.%c", region_name, '\0');
          // }
          // printf("name:%s, poll state:%d\n", region[child_list[index]].name, region[child_list[index]].poll_flag);
          if (region[child_list[index]].poll_flag == 0) { // Not open yet
            region[child_list[index]].poll_flag = 1;
            response = "SC;\0";
          }
          if (region[child_list[index]].poll_flag == 2) { // Already Closed
            region[child_list[index]].poll_flag = region[child_list[index]].poll_flag;
            sprintf(response, "PF;%s:close.%c", region_name, '\0');
          }
        // printf("name:%s, poll state:%d\n", region[child_list[index]].name, region[child_list[index]].poll_flag);
        // printf("name:%s, state:%d\n", region[child_list[index]].name, region[child_list[index]].poll_flag);
        }
        counter =0;
        for(int k = 0; k < 100; k++) {
            child_list[k] = 0;
        }
      }
    }
    if (check_exit == 0) {
      sprintf(response, "NR;%s", region_name);
      strcat(response, "\0");
    }
    // for (int i = 0; region[i].name != NULL; i++) {
    //   printf("check: name:%s, poll state:%d\n", region[i].name, region[i].poll_flag);
    // }

  } else if (strcmp(header, "AV") == 0) { // Add votes
    for (int i = 0; region[i].name != NULL; i++) {
      printf("check: name:%s, poll state:%d\n", region[i].name, region[i].poll_flag);

    }
    printf("***************\n");
    // printf("%s,%s\n", region_name, vote);
    // int i = 0;
    // while(region[i].name != NULL) {
    for(int i = 0; region[i].name != NULL; i++) {
      if (strcmp(region[i].name, trimwhitespace(region_name)) == 0) {
        if(region[i].poll_flag != 1) { // poll not open
          sprintf(response, "RC;%s", region_name);
        } else { // poll open
          printf("region name: %s\n", region[i].name);
          char** data;
          int n = makeargv(vote, ",", &data);
          for (int i = 0; i < n; i++) {
            char** data2;
            int m = makeargv(data[i], ":", &data2);
            // printf("parent:%s\n", region[region[i].parent].name);
            region[i].candi[i].name = data2[0];
            region[i].candi[i].vote = atoi(data2[1]);
            printf("candidate:%s\n", region[i].candi[i].name);
            printf("vote:%d\n", region[i].candi[i].vote);
          }
          response = "SC;";
        }
      }
      // i++;
    }

    printf("test!!!!!!!%s\n", region[region[1].child[0]].candi[1].name);
    // response = "SC;";
    printf("***************\n");
  } else if (strcmp(header, "RV") == 0) { // Remove votes
    response = "SC;";
  } else if (strcmp(header, "CP") == 0) { // Close polls
    int check_exit = 0; // False. There is no such region
    for (int i = 0; region[i].name != NULL; i++) {
      if (strcmp(region[i].name, trimwhitespace(region_name)) == 0) {
        if (region[i].poll_flag == 2) {
          sprintf(response, "PF;%s:close.%c", region_name, '\0');
        }
        check_exit = 1; // True. Region found
        child_list[0] = region[i].id;
        read_children(child_list);
        for (int index = 0; index < counter; index++) {
          // printf("name:%s, state:%d\n", region[child_list[index]].name, region[child_list[index]].poll_flag);
          if(region[child_list[index]].poll_flag == 1) { // Already open
            region[child_list[index]].poll_flag = 2;
            response = "SC;\0";
          }
          if (region[child_list[index]].poll_flag == 0) { // Not open yet
            region[child_list[index]].poll_flag = region[child_list[index]].poll_flag;
            sprintf(response, "PF;%s:unopen.%c", region_name, '\0');
          }

        }
        counter =0;
        for(int k = 0; k < 100; k++) {
            child_list[k] = 0;
        }
      }
    }
    if (check_exit == 0) {
      sprintf(response, "NR;%s", region_name);
      strcat(response, "\0");
    }
  } else{
    sprintf(response, "UC;%s", header); // Unhandled Command
  }
  // for (int i = 0; region[i].name != NULL; i++) {
  //   printf("check: name:%s, poll state:%d\n", region[i].name, region[i].poll_flag);
  //
  // }

  return response;
}


// read the DAG file and load into memory
void read_DAG(char * filename) {
  FILE *dag = fopen(filename, "r");
  if (dag == NULL) {
    perror("Failed to open the DAG file");
    exit(1);
  }
  char *line;
  size_t len = 0;
  ssize_t read;

  region = (struct dnode*) malloc (sizeof(struct dnode) * MAX_REGIONS);
  int list_counter = 0;
  while ((read = getline(&line, &len, dag)) != -1) {
    // if (strcmp(line, "\n") == 0) continue;
    char** array;
    int n = makeargv(trimwhitespace(line), ":", &array);
    // Root node
    if (list_counter == 0)
    {
      region[0].name = array[0]; // should be "Who_Won"
      // printf("parent:%s\n",region[0].name );
      region[0].parent = -1;
      region[0].num_children = n-1;
      list_counter++;
      // region[0].path = temp_path;
      region[0].id = 0;
      // strcat(region[0].path, "/");
      // strcat(region[0].path, region[0].name);

      // children of the root node
      for(int i = 1; i < n; i++)
      {
        region[0].child[i-1] = i;
        region[i].name = array[i];
        // printf("children:%s\n",region[i].name );

        region[i].parent = 0;
        region[i].id = i;
        // region[i].path = malloc(2048);
        // strcpy(region[i].path, region[0].path);
        // strcat(region[i].path, "/");
        // strcat(region[i].path, region[i].name);
        list_counter ++;
      }
    }
    else // Not root node
    {
      for(int i = 0; i < list_counter; i++)
      {
        if(strcmp(region[i].name, array[0]) == 0)
        {
          // printf("parent:%s\n",region[i].name);

          region[i].num_children = n-1;
          for(int j=1; j < n; j++)
          {
            region[i].child[j-1] = list_counter;
            region[list_counter].name = array[j];
            // printf("children:%s\n",region[list_counter].name);
            region[list_counter].parent = i;
            region[list_counter].id = list_counter;
            // region[list_counter].path = malloc(2048);
            // strcpy(region[list_counter].path, region[i].path);
            // strcat(region[list_counter].path, "/");
            // strcat(region[list_counter].path, region[list_counter].name);
            list_counter++;
          }
        }
      }
    }
  }
}

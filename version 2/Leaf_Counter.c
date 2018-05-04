/*login: huxxx990, shixx566
  date: 03/09/18
  name: Hanyu Hu, Xicun Shi
  id: 5272396, 5090878 */


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <libgen.h>
#include "makeargv.h"

#define MAX_CANDIDATES 256

/** This function is to check if there is a file "votes.txt" in the directory.
    If so, it means this is a leaf node, otherwise, this is not a leaf node. **/
int FileCheck(char *path) {
  struct dirent* dirent;
  DIR* dir;
  dir = opendir(path);

  if(dir == NULL) {
    fprintf(stderr, "Can't find this path\n");
    exit(1);
  }
  while ((dirent = readdir(dir)) != NULL) {
    if(dirent->d_type == DT_REG){ // check if this file is regular file
      if(strcmp(dirent->d_name, "votes.txt") == 0){
        return 1;
      }
    }
    else {
      continue;
    }
  }
  closedir(dir);
  printf("Not a leaf node.\n");// otherwise this is not a leaf node
  return 0;
}

/** This function is to parse the inputs in the "votes.txt". It will read the
    inputs in the text file, and count votes for each candidate.**/
int praseInput(char *path) {
  struct dirent* dirent;
  DIR *dir;
  FILE *input, *output;
  size_t size;
  dir = opendir(path);

  // check if the file is the leaf or not, if it is then go ahead to deal with it
  if(FileCheck(path)){
    dirent = readdir(dir);
    char *in_path = malloc(strlen(path) + strlen("votes.txt") + 2);
    char *out_path = malloc(strlen(path) + strlen(basename(path)) + 6);
    char *temp = (char*)malloc(MAX_CANDIDATES * sizeof(char));
    char *buffer = (char*)malloc(1024 * sizeof(char));
    int counter[MAX_CANDIDATES] = {0}; // store the candidate and its vote
    int candi;
    sprintf(in_path, "%s/%s", path, "votes.txt");
    sprintf(out_path, "%s/%s%s", path, basename(path),".txt");
    printf("%s\n", out_path);


    input = fopen(in_path, "r");
    output = fopen(out_path, "w+");
    free(in_path);
    free(out_path);

    if(input != NULL) {
      while (candi = fgetc(input)){
        if (candi != EOF && candi != '\n') {
          counter[candi] += 1;
        }
        else if (candi == EOF) {
          break;
        }
      }
    }

    for (int i = 0; i<MAX_CANDIDATES; i++) {
      if(counter[i] > 0) {
        snprintf(buffer, sizeof(buffer)*4, "%c:%d,", i, counter[i]);
        strcat(temp, buffer);
      }
    }

    temp[strlen(temp)-1] = 0;
    fprintf(output, "%s\n", temp);
    fclose(input);
    fclose(output);
    closedir(dir);
    free(temp);
    free(buffer);
    return 1;
    }

    return 0;
  }

int main(int argc, char** argv){
  if(argc < 2){ //checks for proper number of arguments
    printf("Wrong number of args to leafcounter, expected 1, given %d\n", argc - 1);
		exit(1);
  }
  else{
    praseInput(argv[1]);
  }
  return 0;
}

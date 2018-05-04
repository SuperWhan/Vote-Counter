/*login: huxxx990, shixx566
  date: 03/09/18
  name: Hanyu Hu, Xicun Shi
  id: 5272396, 5090878 */


#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "makeargv.h"


/** This function is to check the current path is whether a leaf node or not. If
    it is a leaf node, it returns 1. Otherwise, it returns 0.
    **/
int leafCheck(char *path){
  DIR *dir;
  struct dirent* dirent;
  dir = opendir(path);

  if(dir == NULL) {
    exit(1);
  }
  while ((dirent = readdir(dir)) != NULL)
  {
    // printf("%s\n", dirent->d_name);
    if(dirent->d_type == DT_DIR && strcmp(dirent->d_name, ".") != 0 && strcmp(dirent->d_name, "..") != 0)
    {
      return 0;
    }
  }
  return 1;
}


/** This function is to aggreagte all the votes from subdirectories. It reads
    the inputs from its children's text files, and count the total votes for
    each candidate, then store the agregation into a text file, which named by
    the current node.
    **/
int AggregateVote(char *path){
  DIR *dir;
  struct dirent* dirent;
  dir = opendir(path);
  dirent = readdir(dir);
  dirent = readdir(dir);

  char* outpath = malloc(strlen(path)+2*strlen(dirent->d_name)+8);

  while ((dirent = readdir(dir)) != NULL)
  {
    if (dirent->d_type == DT_DIR)
    {
      pid_t pid = fork();
      if (pid == 0)
      {
        char* temp = malloc(1024);
        sprintf(outpath, "%s/%s", path, dirent->d_name);
        if(leafCheck(outpath))
        {
          execl("./Leaf_Counter", "Leaf_Counter", outpath, (char*)NULL);
          perror("Excute Leaf_counter failed.\n");
        }
        else
        {
          execl("./Aggregate_Votes", "Aggregate_Votes", outpath, (char*)NULL);
          perror("Excute Aggregate_Votes failed.\n");
          // recursivly call Aggregate_votes, if it is leaf then go precious lines
        }
      }
      else if(pid > 0)
      {

        wait(NULL);
        FILE* infile;
        FILE* outfile;
        char buff[1024];
        sprintf(outpath, "%s/%s/", path, dirent->d_name);
        if(leafCheck(outpath))
        {
          continue;
        }
        else
        {
          char candi;
          char* temp = malloc(1024);
          char* add = malloc(1024);
          int candidates[256] = {0};
          DIR *temp_dir = opendir(outpath);
          struct dirent* temp_dirent;
          strcpy(temp, outpath);
          while ((temp_dirent = readdir(temp_dir)) != NULL)
          {
            if (strcmp(temp_dirent->d_name, ".")!=0 && strcmp(temp_dirent->d_name, "..")!=0)
            {
              if(temp_dirent->d_type==DT_REG)
              {
                continue;
              }
              sprintf(temp, "%s%s/", outpath, temp_dirent->d_name);
              strcpy(add, temp);
              strcat(temp, temp_dirent->d_name);
              strcat(temp, ".txt");
              infile = fopen(temp, "r");
              if(infile != NULL)
              {
                char* line = malloc(1024);
                size_t len = 0;
                ssize_t read;

                while ((read = getline(&line, &len, infile)) != -1)
                {
                  char** array;
                  int n = makeargv(line, ",", &array);

                  for (int i = 0; i < n; i++)
                  {
                    char** temp_c;
                    makeargv(array[i], ":", &temp_c);
                    int number = atoi(temp_c[1]);
                    char t_c = temp_c[0][0];
                    candidates[t_c] += number;
                  }
                }
              }
              if(infile == NULL)
              {
                perror("Failed to open file");
              }
            }
          }


            char* buffer = (char*)malloc(1024 * sizeof(char));
            char* temp_w = (char*)malloc(256 * sizeof(char));
            for (int i = 0; i < 256; i++) {
              if (candidates[i] > 0)
              {
                snprintf(buffer, sizeof(buffer)*4, "%c:%d,", i, candidates[i]);
                strcat(temp_w, buffer);
              }
            }

            char* upperpath = malloc(strlen(outpath) + strlen(basename(outpath)) + 6);
            sprintf(upperpath, "%s/%s%s", outpath, basename(outpath), ".txt");
            outfile = fopen(upperpath, "w+");
            temp_w[strlen(temp_w)-1] = 0;
            fprintf(outfile, "%s\n", temp_w);
            free(buffer);
            free(temp_w);
            free(temp);
            free(add);

            fclose(outfile);
            fclose(infile);
          }

        close(pid);
      }

      else
      {
        fprintf(stderr, "fork error\n");
        return 0;
      }
    }
  }
      return 1;
}


int main(int argc, char** argv)
{
  char directory[1024];
  if (argc != 2)
	{
		printf("Incorrect number of arguments, Expected 1 but %d entered\n", argc - 1);
		return 0;
	}

  if (leafCheck(argv[1]))
  {
    printf("leaf can't get AggregateVote, passing to Leaf_counter\n");
    execl("./Leaf_counter", "Leaf_counter", argv[1], (char*)NULL);
    return 0;
  }

  else
  {
    AggregateVote(argv[1]);
  }
  return 1;
}

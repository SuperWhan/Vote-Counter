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
    if(dirent->d_type == DT_DIR && strcmp(dirent->d_name, ".") != 0 && strcmp(dirent->d_name, "..") != 0)
    {
      return 0;
    }
  }
  return 1;
}

/** This function is to find the maximum index of an array.
  **/
int find_maximum(int a[], int n) {
  int c, max, index;

  max = a[0];
  index = 0;

  for (c = 1; c < n; c++) {
    if (a[c] > max) {
       index = c;
       max = a[c];
    }
  }

  return index;
}

int str_cut(char *str, int begin, int len)
{
    int l = strlen(str);

    if (len < 0) len = l - begin;
    if (begin + len > l) len = l - begin;
    memmove(str + begin, str + begin + len, l - len + 1);

    return 1;
}

int main(int argc, char** argv){
  if(argc != 2){ //checks for proper number of arguments
    printf("Wrong number of args to Vote_Counter, expected 1, given %d\n", argc - 1);
		exit(1);
  }
  else
  {
        pid_t pid = fork();
        char* outpath = malloc(1024);
        char* outpath2 = malloc(1024);
        char* outpath3 = malloc(1024);
        strcpy(outpath, argv[1]);
        strcpy(outpath2, argv[1]);
        strcpy(outpath3, argv[1]);
        int begin = strlen(outpath) - 7;
        int candidates[256] = {0};
        str_cut(outpath, begin, 7);
        str_cut(outpath2, 0, strlen(outpath));

        if (pid == 0)
        {
            if(strcmp(outpath2,"Who_Won")==0)
            {
              execl("./Aggregate_Votes", "Aggregate_Votes", outpath, (char*)NULL);
              perror("Excute Aggregate_Votes failed.\n");
            }
            else
            {
              fprintf(stderr, "This is not root\n");
            }
        }

        else if (pid > 0)
        {
          wait(NULL);
          if (strcmp(outpath2,"Who_Won")==0)
          {
            FILE* infile;
            FILE* outfile;
            strcat(outpath3, "/Who_Won.txt");
            infile = fopen(outpath3, "r");
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

            if (infile == NULL)
            {
              perror("Failed to open file");
            }
            fclose(infile);
            outfile = fopen(outpath3, "a+");
            fprintf(outfile, "Winner: %c\n", find_maximum(candidates, 256));
            fclose(outfile);

          }
          else
          {
            exit(1);
          }
        }

        else
        {
          fprintf(stderr, "fork error\n");
          return 0;
        }
  }

  return 0;
}

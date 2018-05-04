#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "makeargv.h"



struct dnode
{
  char* name;
  char* path;
  int id;
  int parent;
  int child[10];
  int num_childern;
};

struct arguments
{
  pthread_mutex_t* mutex;// segmentation
  struct dnode dn;
  struct dnode *dl;
  char* input_path;
  char* output_path;
};


struct files
{
  char* name;
  struct files* next;
};
struct list
{
  struct files* head;
  struct files* tail;
};

void add_files(struct list *fl, char* file_name)
{
  struct files* temp_node = (struct files*) malloc (sizeof(struct files));
  temp_node->name = file_name;
  temp_node->next = NULL;
  if(fl->head == NULL)
  {
    fl->head = temp_node;
    fl->tail = temp_node;
  }
  else
  {
    fl->tail->next = temp_node;
    fl->tail = temp_node;
  }
}

// struct files *pop_files(struct list *fl)
// {
//   struct files *temp_node = (struct files*) malloc (sizeof(struct files));
//   if(fl->head)
//   {
//     temp_node = fl->head;
//     fl->head = fl->head->next;
//     temp_node->next = NULL;
//     // printf("%s\n", temp_node->name);
//   }
//   else
//   {
//     temp_node = NULL;
//   }
//   return temp_node;
// }

struct list *link_initialize()
{
  struct list *l = (struct list*) malloc (sizeof(struct list));
  l->head = NULL;
  l->tail = NULL;
  return l;
}
//---------------- read and write (s) Leaf counter----------------------------//

int read_write_leaf(struct dnode dl, char* input_path)
{
  char* in_path = malloc(1024);
  char* out_path = malloc(1024);
  char* temp = malloc(256);
  char* buffer = malloc(1024);
  memset(in_path, 0, 1024);
  memset(out_path, 0, 1024);
  memset(temp, 0, 256);
  memset(buffer, 0, 1024);
  int counter[256] = {0};
  int candi;
  FILE *input, *output;

  strcpy(in_path, input_path);
  strcat(in_path, "/");
  strcat(in_path, dl.name);

  strcpy(out_path, dl.path);
  strcat(out_path, "/");
  strcat(out_path, dl.name);
  strcat(out_path, ".txt");
  input = fopen(in_path, "r");
  output = fopen(out_path, "w+");
  while (candi = fgetc(input))
  {

    if (candi != EOF && candi != '\n')
    {
      if(candi == 89 || candi == 90 || candi == 121 || candi == 122)
      {
        snprintf(buffer, sizeof(buffer)*4, "%c\n", candi-24);
        strcat(temp, buffer);
        counter[candi-24] += 1;
      }
      else
      {
        snprintf(buffer, sizeof(buffer)*4, "%c\n", candi+2);
        strcat(temp, buffer);
        counter[candi+2] += 1;
      }
    }
    else if (candi == EOF)
    {
      break;
    }
  }
  temp[strlen(temp)-1] = 0;
  fprintf(output, "%s\n", temp);
  fclose(output);
  fclose(input);
  return 1;
}

//---------------- read and write (e) -------------------------------------//

//---------------- find_maximum helper, return biggest numb in array ---------//
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
//---------------- read and write (s) Aggarate--------------------------------//

int read_write_Agg(struct dnode dn, struct dnode* dl)
{

  char* out_path = malloc(1024);
  char* temp = malloc(256);
  char* buffer = malloc(1024);
  memset(out_path, 0, 1024);
  memset(temp, 0, 256);
  memset(buffer, 0, 1024);
  int counter[256] = {0};
  int candi;
  FILE *input, *output;

  strcpy(out_path, dn.path);
  strcat(out_path, "/");
  strcat(out_path, dn.name);
  strcat(out_path, ".txt");

  char* line = malloc(1024);
  size_t len = 0;
  ssize_t read;

  for(int i=0; i < dn.num_childern; i++)
  {
    if(dl[dn.child[i]].num_childern == 0)
    {
      char* in_path = malloc(1024);
      memset(in_path, 0, 1024);
      strcpy(in_path, dl[dn.child[i]].path);
      strcat(in_path, "/");
      strcat(in_path, dl[dn.child[i]].name);
      strcat(in_path, ".txt");
      input = fopen(in_path, "r");
      while(candi = fgetc(input))
      {
        if (candi != EOF && candi != '\n')
        {
            counter[candi] += 1;
        }
        else if (candi == EOF)
        {
          break;
        }
      }
      fclose(input);
    }
    else
    {
      char* in_path = malloc(1024);
      memset(in_path, 0, 1024);
      strcpy(in_path, dl[dn.child[i]].path);
      strcat(in_path, "/");
      strcat(in_path, dl[dn.child[i]].name);
      strcat(in_path, ".txt");
      input = fopen(in_path, "r");
      while ((read = getline(&line, &len, input)) != -1)
      {
        char** array;
        int m = makeargv(line, "\n", &array);
        for (int i = 0; i < m; i++)
        {
          char** temp_c;
          makeargv(array[i], ":", &temp_c);
          int number = atoi(temp_c[1]);
          char t_c = temp_c[0][0];
          counter[t_c] += number;
        }
      }
      fclose(input);
    }
  }
  for (int i = 0; i < 256; i++)
  {
    if (counter[i] > 0)
    {
      snprintf(buffer, sizeof(buffer)*4, "%c:%d\n", i, counter[i]);
      strcat(temp, buffer);
    }
  }
  output = fopen(out_path, "w+");
  temp[strlen(temp)-1] = 0;
  fprintf(output, "%s\n", temp);
  if(dn.parent == -1)
  {
  fprintf(output, "WINNER:%c\n", find_maximum(counter,256));
  }
  fclose(output);
  return 1;

}

//---------------- read and write (s) -------------------------------------//
//---------------- helper find all leaves ---------------------------------//
int* leaves_finder(struct dnode* list, int l)
{
  static int children[1024];
  int j=0;
  for (int i = 0; i < l; i++)
  {
    if (list[i].num_childern == 0)
    {
      children[j] = i;
      j++;
    }
  }
  return children;
}
//---------------- healper find all leaves --------------------------------//
//---------------- thread process -----------------------------------------//
void* read_write_thread(void *arg)
{
  struct arguments *three_args = (struct arguments*) arg;
  // int* children = leaves_finder(three_args->dl);
  // char* logfile;
  // int tid = pthread_self();
  // int j=0;
  // FILE* output;
  // strcpy(logfile, three_args->output_path);
  // strcat(logfile, "/log.txt");
  read_write_leaf(three_args->dn, three_args->input_path);
  // pthread_mutex_lock(three_args->mutex);
  // if(access(logfile, F_OK) != -1) // file exist
  // {
  //   output = fopen(logfile, "a+");
  // }
  // else
  // {
  //   output = fopen(logfile, "w+");
  // }
  // fprintf(output, "%s:%d:start\n",three_args->dn.name,tid);
  // pthread_mutex_unlock(three_args->mutex);
  while (three_args->dn.parent != -1)
  {
    pthread_mutex_lock(three_args->mutex);
    read_write_Agg(three_args->dl[three_args->dn.parent], three_args->dl);
    three_args->dn = three_args->dl[three_args->dn.parent];
    pthread_mutex_unlock(three_args->mutex);
  }
  // fprintf(output, "%s:%d:end\n",three_args->dn.name,tid);
}
// --------------- thread process -----------------------------------------//
//--------------- main() and  main thread did------------------------------//

int main(int argc, char* argv[])
{

  if (argc < 4)
  {
    printf("Wrong number of args, expected 3, given %d\n", argc - 1);
    exit(1);
  }
  int counter[256] = {0};
  char *graph_path = argv[1];
  char *input_dir = argv[2];
  char *output_dir = argv[3];
  int num_thread = 0;
  int max_thread;
  int max_candidates;
  if(argc == 5)
  {
    max_thread = atoi(argv[4]);
    if(max_thread < 1)
    {
      printf("The thread number can not be nagative or 0");
      exit(1);
    }
  }

  DIR *dir_i;
  struct dirent *dirent_i;
  dir_i = opendir(input_dir);
  if (dir_i == NULL)
  {
    perror("fail to open the input directory");
    exit(1);
  }
//----------------------- generate thread numbers and  file queue (s)----------//
  struct list *list = link_initialize();
  while ((dirent_i = readdir(dir_i)) != NULL)
  {
    if(strcmp(dirent_i->d_name, ".")!=0 && strcmp(dirent_i->d_name, "..")!=0 && strcmp(dirent_i->d_name, ".DS_Store")!=0)
    {
      if(dirent_i->d_type != DT_REG)
      {
        printf("This is not correct input directory, please double checked\n");
        return 0;
      }
      else
      {
        add_files(list, dirent_i->d_name);
        num_thread ++;
      }
    }
  }
  closedir(dir_i);
//----------------------- generate thread numbers and  file queue (e)----------//
//--------------- output directory create (s)----------------------------------//
  DIR *dir_o;
  struct dirent *dirent_o;
  dir_o = opendir(output_dir);


  if(dir_o == NULL)
  {
    mkdir(output_dir, 0777);
  }
//--------------- output directory create (e)-----------------------------------//
//--------------- directory creat based on dag.txt (s)--------------------------//
  FILE *graph = fopen(graph_path, "r");
  if(graph == NULL)
  {
    perror("fail to open the file");
    exit(1);
  }
  char *line;
  size_t len = 0;
  ssize_t read;
  char *temp_path = output_dir;
  struct dnode *d_list = (struct dnode*) malloc (100*sizeof(struct dnode));
  int list_counter = 0;
  while ((read = getline(&line, &len, graph)) != -1)
  {
    char** array;
    int n = makeargv(trimwhitespace(line), ":", &array);
    if (list_counter == 0)
    {
      d_list[0].name = array[0];
      d_list[0].parent = -1;
      d_list[0].num_childern = n-1;
      list_counter++;
      d_list[0].path = output_dir;
      d_list[0].id = 0;
      // sprintf(d_list[0].path, "%s/%s", temp_path, d_list[0].name);
      strcat(d_list[0].path, "/");
      strcat(d_list[0].path, d_list[0].name);
      for(int i = 1; i < n; i++)
      {
        d_list[0].child[i-1] = i;
        d_list[i].name = array[i];
        d_list[i].parent = 0;
        d_list[i].id = i;
        d_list[i].path = malloc(2048);
        strcpy(d_list[i].path, d_list[0].path);
        strcat(d_list[i].path, "/");
        strcat(d_list[i].path, d_list[i].name);
        list_counter ++;
      }
    }


    else
    {
      for(int i = 0; i < list_counter; i++)
      {
        if(strcmp(d_list[i].name, array[0]) == 0)
        {
          d_list[i].num_childern = n-1;
          for(int j=1; j < n; j++)
          {
            d_list[i].child[j-1] = list_counter;
            d_list[list_counter].name = array[j];
            d_list[list_counter].parent = i;
            d_list[list_counter].id = list_counter;
            d_list[list_counter].path = malloc(2048);
            strcpy(d_list[list_counter].path, d_list[i].path);
            strcat(d_list[list_counter].path, "/");
            strcat(d_list[list_counter].path, d_list[list_counter].name);
            list_counter++;
          }
        }
      }
    }
  }
  for(int i =0; i<list_counter; i++)
  {
    if(opendir(d_list[i].path) == NULL)
    {
      mkdir(d_list[i].path, 0777);
    }
  }
  //--------------- directory creat based on dag.txt (e)----------------------//
  int* children = leaves_finder(d_list, list_counter);
  int c = 0;
  while (children[c] != 0)
  {
    struct arguments* arg = (struct arguments*) malloc(sizeof(struct arguments));
    pthread_t pool;
    arg-> dn = d_list[children[c]];
    arg-> dl = d_list;
    arg-> input_path = input_dir;
    arg-> output_path = output_dir;
    arg-> mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(arg->mutex, NULL);
    pthread_create(&pool, NULL, read_write_thread, (void*) arg);
    pthread_join(pool, NULL);
    c++;
  }
  // read_write_leaf(d_list[5], input_dir);
  // read_write_leaf(d_list[6], input_dir);
  // read_write_leaf(d_list[2], input_dir);
  // read_write_leaf(d_list[3], input_dir);
  // // read_write_Agg(d_list[0], d_list[2]);
  // // read_write_Agg(d_list[0], d_list[3]);
  //
  // // read_write_Agg(d_list[1], d_list[4]);
  // read_write_Agg(d_list[4], d_list);
  // read_write_Agg(d_list[1], d_list);
  // read_write_Agg(d_list[0], d_list);
  // // read_write_Agg(d_list[1], d_list[4]);
  // // read_write_Agg(d_list[0], d_list[1]);



  return 1;
}

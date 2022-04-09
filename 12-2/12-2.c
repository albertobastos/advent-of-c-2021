#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINKS 128
#define MAX_NAME 16

struct Cave {
  int index;
  char* name;
  int start;
  int end;
  int small;
  struct Cave* links[MAX_LINKS];
};
typedef struct Cave Cave;

struct List {
  struct Cave* cave;
  struct List* next;
};
typedef struct List List;

List* read_file(FILE*, int*);
Cave* get_start(List*);
void count_paths(Cave*, int, int*);

int main(int argc, char** args) {
  FILE *fp = NULL;
  List* caves = NULL;
  Cave* start = NULL;
  int caves_count = 0;
  int paths_count = 0;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  char* f = args[1];
  //f = "12-2/sample1.in"; // expected = 36
  if ((fp = fopen(f, "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  caves = read_file(fp, &caves_count);
  start = get_start(caves);

  count_paths(start, caves_count, &paths_count);

  printf("Answer: %d\n", paths_count);

  fclose(fp);
  exit(0);
}

Cave* find(List* list, char* name) {
  while(list != NULL) {
    if (strcmp(list->cave->name, name) == 0) return list->cave;
    list = list->next;
  }
  return NULL;
}

int is_big_name(char* name) {
  for (; *name; *name++)
    if (islower(name[0])) return 0;
  return 1;
}

void init_cave(Cave* c, char* name, int index) {
  int i;
  c->index = index;
  c->name = strdup(name);
  c->start = strcmp(name, "start") == 0;
  c->end = strcmp(name, "end") == 0;
  c->small = !is_big_name(name);
  for (i=0; i<MAX_LINKS; i++)
    c->links[i] = NULL;
}

List* find_or_create(List* list, char* name, Cave** cp, int* count) {
  *cp = find(list, name);
  if (*cp == NULL) {
    int i;
    *cp = malloc(sizeof(Cave));
    init_cave(*cp, name, *count);   
    if (list == NULL) {
      list = malloc(sizeof(List));
      list->cave = *cp;
      list->next = NULL;
    } else {
      List* node = malloc(sizeof(List));
      node->cave = *cp;
      node->next = list;
      list = node;
    }
    (*count)++;
  }
  return list;
}

int add_link(Cave* from, Cave* to) {
  int i;
  for (i=0; i<MAX_LINKS; i++) {
    if (from->links[i] == NULL) {
      from->links[i] = to;
      return 1;
    } else if (from->links[i] == to) {
      return 0;
    }
  }
  printf("Max links exceeed adding %s-%s\n", from->name, to->name);
  exit(1);
}

List* read_file(FILE* in, int* count) {
  List* list = NULL;
  char line[MAX_NAME*2+2];
  char* from;
  char* to;
  Cave* c1 = NULL;
  Cave* c2 = NULL;
  while (fscanf(in, "%s", line) == 1) {
    from = strtok(line, "-");
    to = strtok(NULL, "-");
    list = find_or_create(list, from, &c1, count);
    list = find_or_create(list, to, &c2, count);
    add_link(c1, c2);
    add_link(c2, c1);
  }
  return list;
}

Cave* get_start(List* caves) {
  while (caves != NULL) {
    if (caves->cave->start) return caves->cave;
    caves = caves->next;
  }
  return NULL;
}

void print_visited(int n, int* visited) {
  int i;
  printf("visited:");
  for (i=0; i<n; i++)
    printf(" %d", visited[i]);
  printf("\n");
}

void copy_visited(int n, int* target, int* source) {
  int i;
  for (i=0; i<n; i++)
    target[i] = source[i];
}

void count_paths0(Cave* c, int caves_count, int* visited, int small_revisited, int* paths_count) {
  if (visited[c->index] && c->small) {
    if (!c->start && !small_revisited)
      small_revisited = 1;
    else
      return;
  }
  if (c->end) {
    (*paths_count)++;
    return;
  }
  visited[c->index] = 1;

  int i;
  int visited_copy[caves_count];
  for (i=0; c->links[i] != NULL; i++) {
    copy_visited(caves_count, visited_copy, visited);
    count_paths0(c->links[i], caves_count, visited_copy, small_revisited, paths_count);
  }
}

void count_paths(Cave* start, int caves_count, int* paths_count) {
  int visited[caves_count];
  memset(visited, 0, sizeof(visited));
  int small_revisited = 0;
  count_paths0(start, caves_count, visited, small_revisited, paths_count);
}

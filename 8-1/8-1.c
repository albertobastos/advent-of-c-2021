#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
  0   abcefg    6 tracks
  1   cf        2 tracks
  2   acdeg     5 tracks
  3   acdfg     5 tracks
  4   bcdf      4 tracks
  5   abdfg     5 tracks
  6   abdefg    6 tracks
  7   acf       3 tracks
  8   abcdefg   7 tracks
  9   abcdfg    6 tracks
*/

#define PATTERNS 10
#define DISPLAYS 4
#define TRACKS 7

struct Reading {
  struct Reading *next;
  char* patterns[PATTERNS];
  char* displays[DISPLAYS];
};
typedef struct Reading Reading;

Reading *read_file(FILE*);
unsigned long tracks(char*);
int answer(Reading*);

int main(int argc, char** args) {
  FILE *fp = NULL;
  Reading *head = NULL;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  head = read_file(fp);

  printf("Answer: %d\n", answer(head));

  fclose(fp);
  exit(0);
}

Reading *read_file(FILE* in) {
  Reading* head = NULL;
  Reading* curr = NULL;
  char buf[TRACKS+1];
  int i;

  while (!feof(in)) {
    if (head == NULL) {
      head = malloc(sizeof(Reading));
      curr = head;
    } else {
      curr->next = malloc(sizeof(Reading));
      curr = curr->next;
    }

    for (i = 0; i<PATTERNS; i++) {
      fscanf(in, "%s", buf);
      curr->patterns[i] = strdup(buf);
    }
    fscanf(in, "%s", buf);
    if (strcmp(buf, "|") != 0) {
      printf("Error: pipe (|) expected.\n");
      exit(1);
    }
    for (i = 0; i<DISPLAYS; i++) {
      fscanf(in, "%s", buf);
      curr->displays[i] = strdup(buf);
    }    
  }

  return head;
}

unsigned long tracks(char* in) {
  return strlen(in);
}

int answer(Reading* list) {
  int c = 0;
  int i;
  while (list != NULL) {
    for (i=0; i<DISPLAYS; i++) {
      switch (tracks(list->displays[i])) {
        case 2:
        case 4:
        case 3:
        case 7:
          c++;
          break;
        default:
          break;
      }
    }
    list = list->next;
  }

  return c;
}
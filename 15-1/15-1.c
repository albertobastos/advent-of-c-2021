#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COLS 100
#define ROWS 100
#define GRIDSIZE sizeof(int)*COLS*ROWS

void print_grid(int* grid) {
  printf("Grid:\n");
  int i;
  int j;
  for (j=0; j<ROWS; j++) {
    for (i=0; i<COLS; i++) {
      printf("%d", grid[j*COLS+i]);
    }
    printf("\n");
  }
}

int* read_file(FILE* in) {
  int* grid = malloc(GRIDSIZE);
  int i = 0;
  char c;
  while ((c = fgetc(in)) != EOF) {
    if (c == '\n') {
      continue;
    }
    grid[i++] = c - '0';
  }
  return grid;
}

int* calculate_accumulated_risks(int* grid) {
  int* r = malloc(GRIDSIZE);
  memset(r, 0, GRIDSIZE);
  return r;
}

int get_answer(int* accrisks) {
  return -1;
}

int main(int argc, char** args) {
  FILE *fp = NULL;
  int* grid = NULL;
  int* accrisks = NULL;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  grid = read_file(fp);
  accrisks = calculate_accumulated_risks(grid);

  printf("Answer: %d\n", get_answer(accrisks));

  fclose(fp);
  exit(0);
}
#include <stdio.h>
#include <stdlib.h>

#define ROWS 10
#define COLS 10
#define FLASHED -1

int idx(int, int);
void read_grid(FILE*, int*);
void do_step(int*, int*);
void print_grid(int*);

int main(int argc, char** args) {
  FILE *fp = NULL;
  int grid[ROWS*COLS];
  int flashes;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  //args[1] = "11-1/sample.in"; // expected = 1656
  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  read_grid(fp, grid);

  flashes = 0;
  int step;
  for (step=1; step<=100; step++) {
    do_step(grid, &flashes);
  }

  printf("Answer: %d\n", flashes);

  fclose(fp);
  exit(0);
}

int is_valid(int x, int y) {
  return x >= 0 && x < ROWS && y >= 0 && y < COLS;
}

int idx(int x, int y) {
  return x*COLS+y;
}

void read_grid(FILE* in, int* grid) {
  int x;
  int y;
  char line[COLS+1];
  for (x=0; x<ROWS; x++) {
    fscanf(in, "%s", line);
    for (y=0; y<COLS; y++) {
      grid[idx(x,y)] = line[y]-'0';
    }
  }
}

void print_grid(int* grid) {
  int x;
  int y;
  for (x=0; x<ROWS; x++) {
    for (y=0; y<COLS; y++) {
      printf("%d", grid[idx(x,y)]);
    }
    printf("\n");
  }
  printf("\n");
}

void maybe_inc(int* grid, int x, int y) {
  if (is_valid(x, y) && grid[idx(x, y)] != FLASHED)
    grid[idx(x,y)]++;
}

int maybe_flash(int* grid, int x, int y) {
  int i = idx(x,y);
  if (grid[i] > 9) {
    grid[i] = FLASHED;
    maybe_inc(grid, x-1, y-1);
    maybe_inc(grid, x-1, y);
    maybe_inc(grid, x-1, y+1);
    maybe_inc(grid, x, y-1);
    maybe_inc(grid, x, y+1);
    maybe_inc(grid, x+1, y-1);
    maybe_inc(grid, x+1, y);
    maybe_inc(grid, x+1, y+1);
    return 1;
  }
  return 0;
}

void maybe_restore(int* grid, int x, int y) {
  int i = idx(x,y);
  if (grid[i] == FLASHED) grid[i] = 0;
}

void do_step(int* grid, int* flashes) {
  int flashed;
  int x;
  int y;

  for (x=0; x<ROWS; x++) {
    for (y=0; y<COLS; y++) {
      maybe_inc(grid, x, y);
    }
  }

  int any_flash;
  do {
    any_flash = 0;
    for (x=0; x<ROWS; x++) {
      for (y=0; y<COLS; y++) {
        if (maybe_flash(grid, x, y)) {
          (*flashes)++;
          any_flash = 1;
        }
      }
    }
  } while(any_flash);

  for (x=0; x<ROWS; x++) {
    for (y=0; y<COLS; y++) {
      maybe_restore(grid, x, y);
    }
  }
}
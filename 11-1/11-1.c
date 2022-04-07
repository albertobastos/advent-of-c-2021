#include <stdio.h>
#include <stdlib.h>

#define ROWS 10
#define COLS 10
#define FLASHED -1

int* cell(int*, int, int);
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

  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  read_grid(fp, grid);

  flashes = 0;
  int step;
  print_grid(grid);
  for (step = 1; step<=100; step++) {
    do_step(grid, &flashes);
    printf("\n");
    print_grid(grid);
  }

  printf("Answer: %d\n", flashes);

  fclose(fp);
  exit(0);
}

int* cell(int* grid, int x, int y) {
  if (x < 0 || x > ROWS || y < 0 || y > COLS) return NULL;
  return &grid[x*COLS+y];
}

void set(int* grid, int x, int y, int val) {
  int* c = cell(grid, x, y);
  if (c != NULL) *c = val;
}

int get(int* grid, int x, int y) {
  int* c = cell(grid, x, y);
  return c != NULL ? *c : -1;
}

void inc(int* grid, int x, int y) {
  int* c = cell(grid, x, y);
  if (c != NULL && *c != FLASHED) (*c)++;
}

void flash(int* grid, int x, int y) {
  int* c = cell(grid, x, y);
  if (c != NULL) {
    *c = FLASHED;
    inc(grid, x-1, y-1);
    inc(grid, x  , y-1);
    inc(grid, x+1, y-1);
    inc(grid, x-1, y  );
    inc(grid, x+1, y  );
    inc(grid, x-1, y+1);
    inc(grid, x  , y+1);
    inc(grid, x+1, y+1);    
  }
}

int needs_to_flash(int* grid, int x, int y) {
  int* c = cell(grid, x, y);
  return c != NULL && *c != FLASHED && *c > 9;
}

int restore(int* grid, int x, int y) {
  int* c = cell(grid, x, y);
  if (c != NULL && *c == FLASHED) *c = 0;
}

void read_grid(FILE* in, int* grid) {
  int x;
  int y;
  char line[COLS+1];
  for (x=0; x<ROWS; x++) {
    fscanf(in, "%s", line);
    for (y=0; y<COLS; y++) {
      set(grid, x, y, line[y]-'0');
    }
  }
}

void print_grid(int* grid) {
  int x;
  int y;
  for (x=0; x<ROWS; x++) {
    for (y=0; y<COLS; y++) {
      printf(" %d", get(grid, x, y));
    }
    printf("\n");
  }
}

void do_step(int* grid, int* flashes) {
  int flashed;
  int x;
  int y;

  for (x=0; x<ROWS; x++) {
    for (y=0; y<COLS; y++) {
      inc(grid, x, y);
    }
  }

  do {
    flashed = 0;
    for (x=0; x<ROWS; x++) {
      for (y=0; y<COLS; y++) {
        if (needs_to_flash(grid, x, y)) {
          flash(grid, x, y);
          flashed = 1;
          (*flashes)++;
        }
      }
    }
  } while(flashed);

  for (x=0; x<ROWS; x++) {
    for (y=0; y<COLS; y++) {
      restore(grid, x, y);
    }
  }
}
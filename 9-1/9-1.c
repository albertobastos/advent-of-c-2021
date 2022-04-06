#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROWS 100
#define COLS 100

void read_file(FILE*, short*);
int get_answer(short*);

int main(int argc, char** args) {
  FILE *fp = NULL;
  short grid[ROWS*COLS];

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  read_file(fp, grid);
  printf("Answer: %d\n", get_answer(grid));

  fclose(fp);
  exit(0);
}

void read_file(FILE* in, short* grid) {
  int row = 0;
  int col;
  char c;
  while (row < ROWS) {
    for (col=0; col<COLS; col++) {
      c = fgetc(in);
      if (c == '\n') c = fgetc(in);
      grid[row*COLS+col] = c - '0';
    }
    row++;
  }
}

short get(short* grid, int row, int col) {
  if (row < 0 || row >= ROWS || col < 0 || col >= COLS) return 999;
  return grid[row*COLS+col];
}

int get_answer(short* grid) {
  int sum = 0;
  int row;
  int col;
  short val;
  for (row=0; row<ROWS; row++) {
    for (col=0; col<COLS; col++) {
      val = get(grid, row, col);
      if(
        get(grid, row, col-1) > val
        && get(grid, row, col+1) > val
        && get(grid, row-1, col) > val
        && get(grid, row+1, col) > val
      ) {
        sum += val+1;
      }
    }
  }
  return sum;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Point {
  int x;
  int y;
};
typedef struct Point Point;

struct Line {
  struct Point *from;
  struct Point *to;
};
typedef struct Line Line;

struct LineNode {
  Line *line;
  struct LineNode *next;
};
typedef struct LineNode LineNode;

LineNode *read_lines(FILE*, int*, int*);
Line *create_line(int, int, int, int);
int *create_grid(int, int);
void fill_cells(int*, LineNode*, int);
int count_gt(int, int*, int, int);

int main(int argc, char** args) {
  FILE *fp = NULL;
  LineNode *lines = NULL;
  int *grid = NULL;
  int max_x = 0;
  int max_y = 0;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  lines = read_lines(fp, &max_x, &max_y);
  grid = create_grid(max_x, max_y);
  fill_cells(grid, lines, max_y);

  printf("Answer: %d\n", count_gt(1, grid, max_x, max_y));

  fclose(fp);
  exit(0);
}

LineNode *read_lines(FILE* in, int* max_x, int* max_y) {
  LineNode *first = NULL;
  LineNode *curr = NULL;
  LineNode *prev = NULL;
  Line *line = NULL;

  int x1, y1, x2, y2;
  static char* fmt = "%d,%d -> %d,%d";
  while (fscanf(in, fmt, &x1, &y1, &x2, &y2) == 4) {
    line = create_line(x1, y1, x2, y2);
    if (line == NULL) continue;

    curr = malloc(sizeof(LineNode));
    curr->line = line;
    if (line->from->x > *max_x)
      *max_x = line->from->x;
    if (line->from->y > *max_y)
      *max_y = line->from->y;
    if (line->to->x > *max_x)
      *max_x = line->to->x;
    if (line->to->y > *max_y)
      *max_y = line->to->y;
    
    if (first == NULL)
      first = curr;
    else
      prev->next = curr;
    
    prev = curr;
  }

  return first;
}

Line *create_line(int x1, int y1, int x2, int y2) {
  Line *line = malloc(sizeof(Line));
  line->from = malloc(sizeof(Point));
  line->to = malloc(sizeof(Point));
  line->from->x = x1;
  line->from->y = y1;
  line->to->x = x2;
  line->to->y = y2;
  return line;
}

int *create_grid(int x, int y) {
  int total = x*y;
  int *grid = malloc(sizeof(int)*x*y);
  int i;
  for (i=0; i<total; i++)
    grid[i] = 0;
  return grid;
}

void fill_cells(int *grid, LineNode *lines, int cols) {
  int xfrom;
  int xto;
  int yfrom;
  int yto;
  int xstep;
  int ystep;
  while(lines != NULL) {
    xfrom = lines->line->from->x;
    yfrom = lines->line->from->y;
    xto = lines->line->to->x;
    yto = lines->line->to->y;
  
    xstep = xto == xfrom ? 0 : xto > xfrom ? 1 : -1;
    ystep = yto == yfrom ? 0 : yto > yfrom ? 1 : -1;

    while(xfrom != xto || yfrom != yto) {
      grid[xfrom*cols+yfrom]++;
      xfrom += xstep;
      yfrom += ystep;
    }
    grid[xfrom*cols+yfrom]++;
    lines = lines->next;
  }
}

int count_gt(int val, int *grid, int rows, int cols) {
  int count = 0;
  int i;
  for (i=0; i<rows*cols; i++) {
    if (grid[i] > val)
      count++;
  }
  return count;
}

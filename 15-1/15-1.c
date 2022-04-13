#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COLS 100
#define ROWS 100
#define GRIDSIZE sizeof(int)*COLS*ROWS
#define UNVISITED 0
#define VISITED 1
#define INFINITE -1

int idx(int x, int y) {
  return x*COLS+y;
}

void print_state(int* distances, int* visited) {
  int i;
  int j;
  int ij;
  for (j=0; j<ROWS; j++) {
    for (i=0; i<COLS; i++) {
      ij = idx(i,j);
      printf("%4d%c", distances[idx(j,i)], visited[ij] == UNVISITED ? '*' : ' ');
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

int in_bounds(int x, int y) {
  return x >= 0 && y >= 0 && x < COLS && y < ROWS;
}

void update_distance(int* weights, int* distances, int* visited, int from, int x, int y) {
  int xy = idx(x, y);
  if (!in_bounds(x, y)) return;
  if (visited[xy] == VISITED) return;
  int d = from + weights[xy];
  int curr = distances[xy];
  if (curr == INFINITE || curr > d) distances[xy] = d;
}

void visit(int* weights, int* distances, int* visited, int x, int y) {
  int xy = idx(x,y);
  int from = distances[xy];
  update_distance(weights, distances, visited, from, x-1, y);
  update_distance(weights, distances, visited, from, x+1, y);
  update_distance(weights, distances, visited, from, x, y-1);
  update_distance(weights, distances, visited, from, x, y+1);
  visited[xy] = VISITED;
}

void calc_next_current(int* distances, int* visited, int* x, int* y) {
  int mini;
  int minj;
  int mindistance = INFINITE;
  int ij;
  int d;
  for (int i=0; i<COLS; i++) {
    for (int j=0; j<ROWS; j++) {
      ij = idx(i, j);
      if (visited[ij] == UNVISITED) {
        d = distances[ij];
        if (d != INFINITE && (mindistance == INFINITE || mindistance > d)) {
          mindistance = distances[ij];
          mini = i;
          minj = j;
        }
      }
    }
  }
  *x = mini;
  *y = minj;
}

void apply_dijkstra(
  int* weights, int* distances, int* visited,
  int xcurr, int ycurr, int xtarget, int ytarget
) {
  while (xcurr != xtarget || ycurr != ytarget) {
    visit(weights, distances, visited, xcurr, ycurr);
    calc_next_current(distances, visited, &xcurr, &ycurr);
  }
}

int calc_min_distance(int* weights, int xfrom, int yfrom, int xto, int yto)  {
  int* distances = malloc(GRIDSIZE);
  int* visited = malloc(GRIDSIZE);
  memset(distances, INFINITE, GRIDSIZE);
  memset(visited, UNVISITED, GRIDSIZE);
  distances[idx(xfrom, yfrom)] = 0;
  apply_dijkstra(weights, distances, visited, xfrom, yfrom, xto, yto);
  return distances[idx(xto, yto)];
}

int main(int argc, char** args) {
  FILE *fp = NULL;
  int* map = NULL;
  int minrisk;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  //args[1] = "15-1/sample.in";
  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  map = read_file(fp);
  minrisk = calc_min_distance(map, 0, 0, COLS-1, ROWS-1);

  printf("Answer: %d\n", minrisk);

  fclose(fp);
  exit(0);
}
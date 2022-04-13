#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define TILECOLS 10
#define TILEROWS 10
#define REPEATS 5
#define COLS TILECOLS*REPEATS
#define ROWS TILEROWS*REPEATS
#define INFINITE INT_MAX

struct Cell {
  int x;
  int y;
  short weight;
  short visited;
  int distance;
};
typedef struct Cell Cell;

int xy(int x, int y) {
  return y*COLS+x; 
}

Cell* alloc_map() {
  Cell *map = malloc(sizeof(Cell)*COLS*ROWS);
  return map;
}

void print_map(Cell* map, int x1, int y1, int x2, int y2) {
  for (int y=y1; y<y2; y++) {
    for (int x=x1; x<x2; x++) {
      printf("%d", map[xy(x,y)].weight);
    }
    printf("\n");
  }
}

void print_map_full(Cell* map) {
  print_map(map, 0, 0, COLS, ROWS);
}

Cell* read_map(FILE* in) {
  Cell* map = alloc_map();
  int tile_x = 0;
  int tile_y = 0;
  int repeat_x;
  int repeat_y;
  int map_x;
  int map_y;
  int i;
  Cell* cell;
  char c;
  while ((c = fgetc(in)) != EOF) {
    if (c == '\n') continue;
    i = c - '0';
    //map[xy(tile_x, tile_y)] = i;
    for (repeat_x = 0; repeat_x < REPEATS; repeat_x++) {
      for (repeat_y = 0; repeat_y < REPEATS; repeat_y++) {
        cell = malloc(sizeof(Cell));
        cell->x = tile_x + repeat_x*TILECOLS;
        cell->y = tile_y + repeat_y*TILEROWS;
        cell->weight = i + repeat_x + repeat_y;
        if (cell->weight > 9) cell->weight = cell->weight % 9;
        cell->distance = INFINITE;
        cell->visited = 0;
        map[xy(cell->x, cell->y)] = *cell;
      }
    }
    tile_x = (tile_x+1) % TILECOLS;
    if (tile_x == 0) ++tile_y;
  }

  //print_map(map, 0, 0, TILECOLS, TILEROWS);
  print_map_full(map);
  return map;
}

int in_bounds(int x, int y) {
  return x >= 0 && y >= 0 && x < COLS && y < ROWS;
}

int calc_min_distance(Cell* map, int x1, int y1, int x2, int y2) {
  map[xy(x1,y1)].distance = 0;
  return map[xy(x2,y2)].distance;
}

int main(int argc, char** args) {
  FILE *fp = NULL;
  Cell* map = NULL;
  int minrisk;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  args[1] = "15-2/sample.in";
  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  map = read_map(fp);
  minrisk = calc_min_distance(map, 0, 0, COLS-1, ROWS-1);

  printf("Answer: %d\n", minrisk);

  fclose(fp);
  exit(0);
}
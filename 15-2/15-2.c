#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define TILECOLS 100
#define TILEROWS 100
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
  int pq_index;
};
typedef struct Cell Cell;

int xy(int x, int y) {
  return y*COLS+x; 
}

Cell** alloc_map() {
  Cell **map = malloc(sizeof(Cell)*COLS*ROWS);
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

Cell** read_map(FILE* in) {
  Cell** map = alloc_map();
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
        map[xy(cell->x, cell->y)] = cell;
      }
    }
    tile_x = (tile_x+1) % TILECOLS;
    if (tile_x == 0) ++tile_y;
  }

  //print_map(map, 0, 0, TILECOLS, TILEROWS);
  //print_map_full(map);
  return map;
}

int in_bounds(int x, int y) {
  return x >= 0 && y >= 0 && x < COLS && y < ROWS;
}

struct PriorityQueue {
  int size;
  int tail;
  Cell** items;
};
typedef struct PriorityQueue PriorityQueue;

PriorityQueue* init_priority_queue(Cell** map, int x, int y) {
  // generates a Min Binary Heap representing a priority queue with unvisited
  // cells, with the root always being the unvisited cell with the lowest
  // distance
  // initially all cells are unvisited and with Infinite distance except for the
  // starting one, also unvisited but with distance = 0
  int startxy = xy(x,y);
  int currxy;
  int pqi = 0;
  PriorityQueue* pq = malloc(sizeof(PriorityQueue));
  pq->size = COLS*ROWS;
  pq->items = malloc(sizeof(Cell*)*(pq->size));
  pq->items[pqi++] = map[startxy];
  for (int i=0; i<ROWS; i++) {
    for (int j=0; j<COLS; j++) {
      currxy = xy(i,j);
      if(currxy == startxy) continue;
      pq->items[pqi++] = map[currxy];
    }
  }

  for (int i=0; i<pq->size; i++)
    pq->items[i]->pq_index = i;

  return pq;
}

void shiftdown(PriorityQueue* pq, int i) {
  int min;
  int left_i = 2*i + 1;
  int right_i = 2*i + 2;
  Cell* curr = pq->items[i];
  Cell* swap;

  int min_i = i;
  if (left_i < pq->size && pq->items[left_i]->distance < pq->items[min_i]->distance) {
    min_i = left_i;
  }
  if (right_i < pq->size && pq->items[right_i]->distance < pq->items[min_i]->distance) {
    min_i = right_i;
  }

  if (min_i != i) {
    pq->items[i] = pq->items[min_i];
    pq->items[min_i] = curr;
    pq->items[i]->pq_index = i;
    pq->items[min_i]->pq_index = min_i;
    shiftdown(pq, min_i);
  }
}

int pq_parent(int i) {
  return (i-1)/2;
} 

void shiftup(PriorityQueue* pq, int i) {
  int parent_i;
  Cell* swap = pq->items[i];
  while (i > 0 && pq->items[i]->distance < pq->items[pq_parent(i)]->distance) {
    swap = pq->items[i];
    pq->items[i] = pq->items[pq_parent(i)];
    pq->items[i]->pq_index = i;
    i = pq_parent(i);
    pq->items[i] = swap;
    pq->items[i]->pq_index = i;
  }
}

Cell* pop(PriorityQueue* pq) {
  Cell* head = pq->items[0];
  pq->items[0] = pq->items[pq->size-1];
  (pq->size)--;
  shiftdown(pq, 0);
  return head;
}

void update_distance(Cell** map, PriorityQueue* pq, int from, int x, int y) {
  Cell* c;
  if (!in_bounds(x, y)) return;
  c = map[xy(x, y)];
  if (c->visited) return;
  int newDistance = from + c->weight;
  if (newDistance < c->distance) {
    c->distance = newDistance;
    shiftup(pq, c->pq_index);
  }
}

void visit(Cell** map, PriorityQueue* pq, Cell* c) {
  int d = c->distance;
  int x = c->x;
  int y = c->y;
  update_distance(map, pq, d, x-1, y);
  update_distance(map, pq, d, x+1, y);
  update_distance(map, pq, d, x, y-1);
  update_distance(map, pq, d, x, y+1);
  c->visited = 1;
}

void apply_dijkstra(Cell** map, PriorityQueue* pq, int trg_x, int trg_y) {
  Cell* c = pop(pq);
  while (c->x != trg_x || c->y != trg_y) {
    visit(map, pq, c);
    c = pop(pq);
  }
}

int calc_min_distance(Cell** map, int x1, int y1, int x2, int y2) {
  map[xy(x1,y1)]->distance = 0;
  PriorityQueue* pq = init_priority_queue(map, x1, y1);
  apply_dijkstra(map, pq, x2, y2);
  return map[xy(x2,y2)]->distance;
}

int main(int argc, char** args) {
  FILE *fp = NULL;
  Cell** map = NULL;
  int minrisk;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  //args[1] = "15-2/sample.in";
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
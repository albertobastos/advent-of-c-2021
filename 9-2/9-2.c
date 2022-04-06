#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROWS 100
#define COLS 100

#define VISITED -1
#define TOP 9

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
  if (row < 0 || row >= ROWS || col < 0 || col >= COLS) return TOP;
  return grid[row*COLS+col];
}

void visit(short* grid, int row, int col) {
  grid[row*COLS+col] = VISITED;
}

int expandBasinSize(short* grid, int row, int col) {
  int val = get(grid, row, col);
  if (val == VISITED || val == TOP) return 0;
  visit(grid, row, col);
  return 1
    + expandBasinSize(grid, row-1, col)
    + expandBasinSize(grid, row+1, col)
    + expandBasinSize(grid, row, col-1)
    + expandBasinSize(grid, row, col+1);
}

struct Node {
  int val;
  struct Node* next;
};
typedef struct Node Node;

Node* insert_high2low(Node* head, int v) {
  if (head == NULL) {
    head = malloc(sizeof(Node));
    head->val = v;
    return head;
  } else if (head->val > v) {
    head->next = insert_high2low(head->next, v);
    return head;
  } else {
    Node* curr = malloc(sizeof(Node));
    curr->val = v;
    curr->next = head;
    return curr;
  }
}

int get_answer(short* grid) {
  int sum = 0;
  int row;
  int col;
  short val;
  int size;
  Node* head = NULL;

  for (row=0; row<ROWS; row++) {
    for (col=0; col<COLS; col++) {
      val = get(grid, row, col);
      if (val == VISITED) continue;
      if(
        get(grid, row, col-1) > val
        && get(grid, row, col+1) > val
        && get(grid, row-1, col) > val
        && get(grid, row+1, col) > val
      ) {
        // low point found, calculate basin size
        visit(grid, row, col);
        size = 1
          + expandBasinSize(grid, row-1, col)
          + expandBasinSize(grid, row+1, col)
          + expandBasinSize(grid, row, col-1)
          + expandBasinSize(grid, row, col+1);

        head = insert_high2low(head, size);
      }
    }
  }

  return head->val * head->next->val * head->next->next->val;
}
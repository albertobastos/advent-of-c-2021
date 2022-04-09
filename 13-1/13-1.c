#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 128
#define MARK -1

struct XY {
  int x;
  int y;
  struct XY* next;
};
typedef struct XY XY;

struct State {
  struct XY* dots;
  struct XY* folds;
};
typedef struct State State;

void read_file(FILE*, State*);
void apply_fold(State*, XY*);
int unique_in_canvas(XY*);

int main(int argc, char** args) {
  FILE *fp = NULL;
  State* state = malloc(sizeof(State));
  char* file = NULL;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  file = args[1];
  //file = "13-1/sample.in";
  if ((fp = fopen(file, "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  read_file(fp, state);
  apply_fold(state, state->folds);
  
  printf("Answer: %d\n", unique_in_canvas(state->dots));

  fclose(fp);
  exit(0);
}

void read_dots(FILE* in, State* state) {
  state->dots = NULL;
  fseek(in, 0, 0);
  int x;
  int y;
  XY* dot = NULL;
  XY* tail = NULL;

  while (fscanf(in, "%d,%d\n", &x, &y) == 2) {
    dot = malloc(sizeof(XY));
    dot->x = x;
    dot->y = y;
    dot->next = NULL;

    if (tail == NULL) {
      tail = dot;
      state->dots = tail;
    } else {
      tail->next = dot;
      tail = dot;
    }
  }
}

void read_folds(FILE* in, State* state) {
  state->folds = NULL;
  char dir;
  int coord;
  int read;
  XY* fold = NULL;
  XY* tail = NULL;

  while (fscanf(in, "fold along %c=%d\n", &dir, &coord) == 2) {
    fold = malloc(sizeof(XY));
    if (dir == 'x') {
      fold->x = coord;
      fold->y = MARK;
    } else {
      fold->x = MARK;
      fold->y = coord;
    }
    fold->next = NULL;

    if (tail == NULL) {
      tail = fold;
      state->folds = tail;
    } else {
      tail->next = fold;
      tail = fold;
    }
  }
}

void read_file(FILE* in, State* state) {
  read_dots(in, state);
  read_folds(in, state);
}

int in_canvas(XY* dot) {
  return dot->x >= 0 && dot->y >= 0;
}

void apply_fold(State* state, XY* fold) {
  XY* dot = state->dots;
  for (; dot != NULL; dot=dot->next) {
    //printf("%4d,%4d", dots->x, dots->y);
    if (in_canvas(dot)) {
      if (fold->x == dot->x || fold->y == dot->y) {
        // dot is at the very fold, it banishes
        dot->x = MARK;
      } else if (fold->x != MARK && fold->x < dot->x) {
        // affected by vertical fold
        dot->x = fold->x - (dot->x - fold->x);
      } else if (fold->y != MARK && fold->y < dot->y) {
        // affected by horizontal fold
        dot->y = fold->y - (dot->y - fold->y);
      }
    } else {
      // dot already outside canvas, skip it
    }
    //printf(" --> %4d,%4d\n", dots->x, dots->y);
  }
}

// cantor pairing
int dot_hash(XY* dot) {
  int x = dot->x;
  int y = dot->y;
  return (x + y) * (x + y + 1) / 2 + x;
}

struct Heap {
  int value;
  struct Heap* left;
  struct Heap* right;
};
typedef struct Heap Heap;

Heap* add_if_unique(Heap* root, int value, int* added_count) {
  if (root == NULL) {
    Heap* leaf = malloc(sizeof(Heap));
    leaf->value = value;
    leaf->left = NULL;
    leaf->right = NULL;
    (*added_count)++;
    return leaf;
  } else if (root->value > value) {
    root->left = add_if_unique(root->left, value, added_count);
  } else if (root->value < value) {
    root->right = add_if_unique(root->right, value, added_count);
  } else {
    // root->value == value
  }
  return root;
}

char* whites(int n) {
  char* str = malloc(sizeof(char)*(n+1));
  for (int i=0;i<=n;i++)
    str[i] = ' ';
  str[n] = '\0';
  return str;
}

void print_heap(int spaces, Heap* node) {
  if (node == NULL) return;
  printf("%s- %d\n", whites(spaces), node->value);
  printf("%s(left)\n", whites(spaces));
  print_heap(spaces+1, node->left);
  printf("%s(right)\n", whites(spaces));
  print_heap(spaces+1, node->right);
}

int unique_in_canvas(XY* dots) {
  Heap* h = NULL;
  int hash;
  int count = 0;

  for (; dots != NULL; dots=dots->next) {
    if (in_canvas(dots)) {
      hash = dot_hash(dots);
      //printf("counting %d,%d with hash %d\n", dots->x, dots->y, hash);
      h = add_if_unique(h, hash, &count);
    }
  }

  return count;
}

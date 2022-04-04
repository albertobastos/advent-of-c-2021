#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Version of 5-2 using a binary heap instead of a full grid,
// which leads to a smaller memory footprint

struct Node {
  int x;
  int y;
  int count;
  struct Node *left;
  struct Node *right;
};
typedef struct Node Node;

Node *read_lines(FILE*);
Node *add_line(Node*, int, int, int, int);
Node *add_point(Node*, int, int);
Node *new_point(int, int);
int count_gt(Node*, int);
int tree_size(Node*);

int main(int argc, char** args) {
  FILE *fp = NULL;
  Node *root = NULL;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  root = read_lines(fp);

  printf("Answer: %d\n", count_gt(root, 1));

  fclose(fp);
  exit(0);
}

Node *read_lines(FILE* in) {
  Node *root = NULL;
  Node *curr = NULL;

  int x1, y1, x2, y2;
  static char* fmt = "%d,%d -> %d,%d";
  while (fscanf(in, fmt, &x1, &y1, &x2, &y2) == 4) {
    root = add_line(root, x1, y1, x2, y2);
    //printf("%d nodes after %d,%d -> %d,%d\n", tree_size(root), x1, y1, x2, y2);
  }

  return root;
}

Node *add_line(Node *root, int x1, int y1, int x2, int y2) {
  int xstep = x1 == x2 ? 0 : x1 < x2 ? 1 : -1;
  int ystep = y1 == y2 ? 0 : y1 < y2 ? 1 : -1;

  while (x1 != x2 || y1 != y2) {
    root = add_point(root, x1, y1);
    x1 += xstep;
    y1 += ystep;
  }
  root = add_point(root, x1, y1);
  return root;
}

Node *add_point(Node *root, int x, int y) {
  if (root == NULL) {
    root = new_point(x, y);
  } else if (root->x > x) {
    root->left = add_point(root->left, x, y);
  } else if (root->x < x) {
    root->right = add_point(root->right, x, y);
  } else if (root->y > y) {
    root->left = add_point(root->left, x, y);
  } else if(root->y < y) {
    root->right = add_point(root->right, x, y);
  } else {
    root->count++;
  }
  return root;
}

Node *new_point(int x, int y) {
  Node* p = malloc(sizeof(Node));
  p->x = x;
  p->y = y;
  p->count = 1;
  p->left = NULL;
  p->right = NULL; 
  return p;
}

int count_gt(Node* node, int val) {
  if (node == NULL) return 0;
  return (node->count > val ? 1 : 0)
    + count_gt(node->left, val)
    + count_gt(node->right, val);
}

int tree_size(Node* node) {
  if (node == NULL) return 0;
  return 1 + tree_size(node->left) + tree_size(node->right);
}
#include <stdio.h>
#include <stdlib.h>

#define PAIR -1
#define MAX_EXPR 200

struct SN {
  int regular;
  struct SN* left;
  struct SN* right;
  struct SN* parent;
};
typedef struct SN SN;

SN* add(SN* a, SN* b) {
  SN* sum = malloc(sizeof(SN));
  sum->regular = PAIR;
  sum->left = a;
  sum->right = b;
  sum->parent = NULL;
  sum->left->parent = sum;
  sum->right->parent = sum;
  return sum;
}

SN* new_regular(int regular, SN* parent) {
  SN* n = malloc(sizeof(SN));
  n->regular = regular;
  n->left = NULL;
  n->right = NULL;
  n->parent = parent;
  return n;
}

SN* find_rightmost_leftfrom(SN* curr) {
  if (curr->parent != NULL && curr->parent->right == curr) {
    curr = curr->parent->left;
    while (curr->regular == PAIR)
      curr = curr->right;
    return curr;
  }
  if (curr->parent == NULL) return NULL;
  return find_rightmost_leftfrom(curr->parent);
}

SN* find_leftmost_rightfrom(SN* curr) {
  if (curr->parent != NULL && curr->parent->left == curr) {
    curr = curr->parent->right;
    while (curr->regular == PAIR)
      curr = curr->left;
    return curr;
  }
  if (curr->parent == NULL) return NULL;
  return find_leftmost_rightfrom(curr->parent);
}

int maybe_explode(SN* n, int depth) {
  SN* tmp;
  if (n->regular != PAIR) return 0;
  if (depth == 4) {
    tmp = find_rightmost_leftfrom(n);
    if (tmp != NULL) tmp->regular += n->left->regular;
    tmp = find_leftmost_rightfrom(n);
    if (tmp != NULL) tmp->regular += n->right->regular;
    n->regular = 0;
    free(n->left);
    free(n->right);
    n->left = NULL;
    n->right = NULL;
    return 1;
  }

  return maybe_explode(n->left, depth+1)
      || maybe_explode(n->right, depth+1);
}

int maybe_split(SN* n) {
  if (n->regular == PAIR)
    return maybe_split(n->left) || maybe_split(n->right);
  if (n->regular < 10)
    return 0;

  n->left = new_regular(n->regular / 2, n);
  n->right = new_regular(n->regular - n->left->regular, n);
  n->regular = PAIR;
  return 1;
}

int reduce0(SN* n) {
  if (maybe_explode(n, 0)) return 1;
  return maybe_split(n);
}

void reduce(SN* n) {
  while (reduce0(n)) {};
}

int magnitude(SN* n) {
  if (n->regular != PAIR) return n->regular;
  else return 3*magnitude(n->left) + 2*magnitude(n->right);
}

void print_sn0(SN* n) {
  if (n->regular == PAIR) {
    printf("[");
    print_sn0(n->left);
    printf(",");
    print_sn0(n->right);
    printf("]");
  } else {
    printf("%d", n->regular);
  }
}

void print_sn(SN* n) {
  print_sn0(n);
  printf("\n");
}

SN* parse_sn0(char** str) {
  SN* n = malloc(sizeof(SN));
  char c = (*str)[0];
  if (c == '[') {
    n->regular = PAIR;
    (*str)++;
    n->left = parse_sn0(str);
    (*str)++; // skip comma
    n->right = parse_sn0(str);
    (*str)++; // skip closing bracket
    n->left->parent = n;
    n->right->parent = n;
  } else {
    n->regular = c - '0';
    n->left = NULL;
    n->right = NULL;
    (*str)++;
  }
  return n;
}

SN* parse_sn(char* str) {
  SN* n = parse_sn0(&str);
  //print_sn(n);
  return n;
}

SN** read_file(FILE* in) {
  ssize_t len = 0;
  char* buf;
  SN** list = malloc(sizeof(SN*) * MAX_EXPR);
  
  int i = 0;
  int read;
  while (getline(&buf, &len, in) >= 0) {
    list[i++] = parse_sn(buf);
  }
  
  list[i++] = NULL;
  return list;
}

int main(int argc, char** args) {
  FILE *fp = NULL;
  SN** sns = NULL;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  //args[1] = "18-1/sample.in";
  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  sns = read_file(fp);
  SN* n = sns[0];
  for (int i=1; sns[i] != NULL; i++) {
    n = add(n, sns[i]);
    reduce(n);
  }

  printf("Answer: %d\n", magnitude(n));

  fclose(fp);
  exit(0);
}

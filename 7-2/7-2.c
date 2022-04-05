#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct Node {
  int value;
  struct Node *next;
};
typedef struct Node Node;

Node *read_file(FILE*, int*, int*);
int eval(Node*, int);

int main(int argc, char** args) {
  FILE *fp = NULL;
  Node *head = NULL;
  int min;
  int max;
  int i;
  int result;
  int tmp;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  head = read_file(fp, &min, &max);

  result = -1;
  for(i = min; i<=max; i++) {
    tmp = eval(head, i);
    if (result == -1 || result > tmp)
      result = tmp;
  }

  printf("Answer: %d\n", result);

  fclose(fp);
  exit(0);
}

Node *read_file(FILE* in, int* min, int* max) {
  Node* head;
  Node* curr;
  int val;

  *min = -1;
  *max = -1;

  while(fscanf(in, "%d,", &val) > 0) {
    if (head == NULL) {
      head = malloc(sizeof(Node));
      head->value = val;
      curr = head;
    } else {
      curr->next = malloc(sizeof(Node));
      curr->next->value = val;
      curr = curr->next;
    }
    if (*min == -1 || *min > val)
      *min = val;
    if (*max == -1 || *max < val)
      *max = val;
  }
  return head;
}

int eval(Node* head, int pos) {
  int r = 0;
  int tmp = 0;
  for( ; head != NULL; head = head->next) {
    tmp = abs(head->value - pos);
    r += tmp*(tmp+1)/2;
  }
  return r;
}
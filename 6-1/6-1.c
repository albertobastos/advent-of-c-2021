#include <stdio.h>
#include <stdlib.h>

#define DAYS 80

struct Node {
  int timer;
  struct Node* next;
};
typedef struct Node Node;

Node *read_file(FILE*);
Node *step(Node*);
int count_nodes(Node*);
void print_list(Node*);

int main(int argc, char** args) {
  FILE *fp = NULL;
  Node *head = NULL;
  int i = 0;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  head = read_file(fp);
  //print_list(head);

  for(i=0;i<DAYS;i++)
    head = step(head);
  
  printf("Answer: %d\n", count_nodes(head));

  fclose(fp);
  exit(0);
}

Node *read_file(FILE* in) {
  int timer;
  Node *head = NULL;
  Node *tmp = NULL;
  while (fscanf(in, "%d,", &timer) > 0) {
    tmp = malloc(sizeof(Node));
    tmp->next = head;
    tmp->timer = timer;
    head = tmp;
  }
  return head;
}

Node *step(Node* head) {
  Node *curr = head;
  Node *tmp = NULL;
  while(curr != NULL) {
    curr->timer--;
    if (curr->timer < 0) {
      // add new Node next to its "creator"...
      curr->timer = 6;
      tmp = malloc(sizeof(Node));
      tmp->timer = 8;
      tmp->next = curr->next;
      curr->next = tmp;
      // ... and skip it for this step
      curr = tmp;
    }
    curr = curr->next;
  }
  return head;
}

int count_nodes(Node* head) {
  int count = 0;
  while (head != NULL) {
    count++;
    head = head->next;
  }
  return count;
}

void print_list(Node* head) {
  while(head != NULL) {
    printf("%d,", head->timer);
    head = head->next;
  }
  printf("\n");
}
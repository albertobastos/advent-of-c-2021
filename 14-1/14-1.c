#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOT_FOUND 1

typedef char Component;

struct Node {
  Component item;
  struct Node* next;
};
typedef struct Node Node;

struct Rule {
  Component first;
  Component second;
  Component result;
  struct Rule* next;
};
typedef struct Rule Rule;

struct State {
  Node* template;
  Rule* rules;
};
typedef struct State State;

State* init_state() {
  State* s = malloc(sizeof(State));
  s->template = NULL;
  s->rules = NULL;
}

void read_file(FILE* in, State* state) {
  Component c1, c2, c3;
  Node* nodeTail = NULL;
  Rule* ruleTail = NULL;
  for (;;) {
    c1 = (char)fgetc(in);
    if (c1 == '\n') {
      fgetc(in); // skip second line break
      break;
    }

    if (nodeTail == NULL) {
      nodeTail = malloc(sizeof(Node));
      state->template = nodeTail;
    } else {
      nodeTail->next = malloc(sizeof(Node));
      nodeTail = nodeTail->next;
    }
    nodeTail->item = c1;
    nodeTail->next = NULL;
  }

  while (fscanf(in, "%c%c -> %c\n", &c1, &c2, &c3) == 3) {
    if (ruleTail == NULL) {
      ruleTail = malloc(sizeof(Rule));
      state->rules = ruleTail;
    } else {
      ruleTail->next = malloc(sizeof(Rule));
      ruleTail = ruleTail->next;
    }
    ruleTail->first = c1;
    ruleTail->second = c2;
    ruleTail->result = c3;
    ruleTail->next = NULL;
  }
}

Component find_polymer(State* state, Component first, Component second) {
  for (Rule* r=state->rules; r != NULL; r=r->next)
    if(r->first == first && r->second == second) return r->result;
  return NOT_FOUND;
}

void apply_step(State* state) {
  Node* first = NULL;
  Node* second = NULL;
  Component c;
  for (
      first = state->template, second = first->next
    ; second != NULL
    ; first = second, second = second->next
  ) {
    if ((c = find_polymer(state, first->item, second->item)) != NOT_FOUND) {
      first->next = malloc(sizeof(Node));
      first->next->item = c;
      first->next->next = second;
    } 
  }
}

void print_state(State* state, int template, int rules) {
  if(template) {
    Node* n = state->template;
    printf("Template:\n  ");
    for (Node* n = state->template; n != NULL; n=n->next)
      printf("%c", n->item);
    printf("\n");
  }

  if(rules) {
    printf("Rules:\n");
    for (Rule* r = state->rules; r != NULL; r=r->next)
      printf("  %c%c -> %c\n", r->first, r->second, r->result);
    printf("\n");
  }
}

struct Count {
  Component item;
  int n;
  struct Count* next;
};
typedef struct Count Count;

int answer_part1(State* state) {
  Count* counts = NULL;
  for (Node* n=state->template; n != NULL; n=n->next) {
    Count* c = NULL;
    for (c=counts; c != NULL && c->item != n->item; c=c->next) {}
    if (c == NULL) {
      c = malloc(sizeof(Count));
      c->item = n->item;
      c->n = 0;
      c->next = counts;
      counts = c;
    } else {
      (c->n)++;
    }
  }

  int max = -1;
  int min = -1;

  for(Count* c=counts; c != NULL; c=c->next) {
    if (max == -1 || max < c->n) max = c->n;
    if (min == -1 || min > c->n) min = c->n;
  }

  return max-min;
}

int main(int argc, char** args) {
  char* file = NULL;
  FILE *fp = NULL;
  State* state = init_state();

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  file = args[1];
  //file = "14-1/sample.in";
  if ((fp = fopen(file, "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  read_file(fp, state);

  for (int i=0; i<10; i++) {
    apply_step(state);
    //print_state(state, 1, 0);
  }

  printf("Answer: %d\n", answer_part1(state));

  fclose(fp);
  exit(0);
}

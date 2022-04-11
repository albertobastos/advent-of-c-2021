#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_POLYMERS 1000
#define MARK '0'

typedef char Component;
typedef unsigned long Count;
typedef Component PolymerCode[3];

struct Polymer {
  PolymerCode code;
  PolymerCode* insertions;
  Count count;
  Count delta;
};
typedef struct Polymer Polymer;

struct ComponentCount {
  Component value;
  Count count;
  struct ComponentCount* next;
};
typedef struct ComponentCount ComponentCount;

struct State {
  Polymer* polymers[MAX_POLYMERS];
};
typedef struct State State;

State* init_state() {
  State* s = malloc(sizeof(State));
  memset(s->polymers, 0, sizeof(s->polymers));
}

int eq(PolymerCode a, PolymerCode b) {
  return strcmp(a, b) == 0;
}

Polymer* get_polymer(State* state, PolymerCode code, int create) {
  int idx;
  for (idx=0; state->polymers[idx] != NULL && !eq(state->polymers[idx]->code, code); idx++) {}
  Polymer* p = state->polymers[idx];
  //printf("get %s: %p\n", code, p);
  if (p == NULL) {
    if (!create) {
      printf("Error: polymer %s does not exist.\n", code);
      exit(1);
    }
    p = malloc(sizeof(Polymer));
    strcpy(p->code, code);
    p->count = 0;
    p->delta = 0;
    state->polymers[idx] = p;
  }
  return p;
}

void print(State* s) {
  int i;
  for(i=0; i<MAX_POLYMERS && s->polymers[i] != NULL; i++) {
    printf("%s // %lu\n", s->polymers[i]->code, s->polymers[i]->count);
  }
  printf("%d polymer(s) found\n", i);
}

State* read_file(FILE* in) {
  State* s = init_state();
  ssize_t len = 0;
  char* init = NULL;
  PolymerCode code;
  char to;
  int i;
  Polymer* p;

  getline(&init, &len, in);

  i = 0;
  while (fscanf(in, "%s -> %c\n", code, &to) == 2) {
    p = get_polymer(s, code, 1);
    p->insertions = malloc(sizeof(PolymerCode)*2);
    sprintf(p->insertions[0], "%c%c", code[0], to);
    sprintf(p->insertions[1], "%c%c", to, code[1]);
  }

  for (i=0; init[i+1] != '\n'; i++) {
    sprintf(code, "%c%c", init[i], init[i+1]);
    p = get_polymer(s, code, 0);
    (p->count)++;
  }

  // fake polymers to keep track of the first and last components
  sprintf(code, "%c%c", MARK, init[0]);
  p = get_polymer(s, code, 1);
  p->insertions = NULL;
  (p->count)++;
  sprintf(code, "%c%c", init[i], MARK);
  p = get_polymer(s, code, 1);
  p->insertions = NULL;
  (p->count)++;

  return s;
}

void apply_step(State* state) {
  Polymer* p = NULL;
  for (int i=0; i<MAX_POLYMERS && state->polymers[i] != NULL; i++) {
    p = state->polymers[i];
    if (p->insertions != NULL) {
      (get_polymer(state, p->insertions[0], 0)->delta)+=p->count;
      (get_polymer(state, p->insertions[1], 0)->delta)+=p->count;
      p->count = 0;
    }
  }

  for (int i=0; i<MAX_POLYMERS && state->polymers[i] != NULL; i++) {
    p = state->polymers[i];
    p->count += p->delta;
    p->delta = 0;
  }
}

ComponentCount* add_ccount(ComponentCount* counts, Component comp, Count n) {
  if (comp == MARK) return counts;
  ComponentCount* curr = NULL;
  for (curr=counts; curr != NULL && curr->value != comp; curr=curr->next) {}
  if (curr == NULL) {
    curr = malloc(sizeof(ComponentCount));
    curr->value = comp;
    curr->count = n;
    curr->next = counts;
    counts = curr;
  } else {
    (curr->count) += n;
  }
  return counts;
}

Count answer_part2(State* state) {
  // get component count based on polymer counts
  int i;
  ComponentCount* counts = NULL;
  for (i=0; i<MAX_POLYMERS && state->polymers[i] != NULL; i++) {
    counts = add_ccount(counts, state->polymers[i]->code[0], state->polymers[i]->count);
    counts = add_ccount(counts, state->polymers[i]->code[1], state->polymers[i]->count);
  }

  Count min = counts->count;
  Count max = counts->count;
  for (counts=counts->next; counts != NULL; counts=counts->next) {
    if (min > counts->count) min = counts->count;
    if (max < counts->count) max = counts->count;
  }

  // components are counted twice, one per polymer!
  return max/2 - min/2;
}

int main(int argc, char** args) {
  char* file = NULL;
  FILE *fp = NULL;
  State* state = NULL;

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

  state = read_file(fp);
  for (int i=0; i<40; i++) {
    apply_step(state);
  }

  printf("Answer: %lu\n", answer_part2(state));

  fclose(fp);
  exit(0);
}

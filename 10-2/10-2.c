#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE 1024

typedef unsigned long long Score;

struct ScoreReg {
  Score score;
  struct ScoreReg* next;
};
typedef struct ScoreReg ScoreReg;

ScoreReg* add_autocomplete_score(ScoreReg*, char*);
Score choose_middle_score(ScoreReg*);

int main(int argc, char** args) {
  FILE *fp = NULL;
  char line[1024] = {0};
  size_t len;
  ScoreReg* scores = NULL;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  while (fgets(line, MAX_LINE, fp))
    scores = add_autocomplete_score(scores, line);

  printf("Answer: %llu\n", choose_middle_score(scores));

  fclose(fp);
  exit(0);
}

struct Node {
  char elem;
  struct Node* prev;
};
typedef struct Node Node;

struct Stack {
  struct Node* top;
};
typedef struct Stack Stack;

void push(Stack* s, char elem) {
  Node* n = malloc(sizeof(Node));
  n->elem = elem;
  n->prev = s->top;
  s->top = n;
}

char pop(Stack* s) {
  if (s->top == NULL) return 0;
  Node* top = s->top;
  char elem = top->elem;
  s->top = top->prev;
  free(top);
  return elem;
}

ScoreReg* insert_ordered(ScoreReg* scores, Score score) {
  ScoreReg* sr = malloc(sizeof(ScoreReg));
  sr->score = score;
  if (scores == NULL) {
    sr->next = NULL;
    return sr;
  }

  ScoreReg* curr = scores;
  ScoreReg* prev = NULL;
  for (; curr != NULL && curr->score < score; prev=curr,curr=curr->next) {}
  
  // insert between prev and curr
  
  if (prev == NULL) { // is the new head!
    sr->next = curr;
    return sr;
  }

  sr->next = curr;
  prev->next = sr;

  return scores;
}

ScoreReg* add_autocomplete_score(ScoreReg* scores, char* in) {
  Stack st = {NULL};
  char c;
  for (; *in; *in++) {
    switch (*in) {
      case '(':
      case '[':
      case '{':
      case '<':
        push(&st, *in);
        break;
      case ')':
      case ']':
      case '}':
      case '>':
        c = pop(&st);
        if (*in == ')' && c != '(') return scores;
        if (*in == ']' && c != '[') return scores;
        if (*in == '}' && c != '{') return scores;
        if (*in == '>' && c != '<') return scores;
        break;
      default:
        break;
    }
  }

  Score score = 0;
  while (st.top != NULL) {
    c = pop(&st);
    score *= 5;
    switch (c) {
      case '(':
        score += 1;
        break;
      case '[':
        score += 2;
        break;
      case '{':
        score += 3;
        break;
      case '<':
        score += 4;
        break;
      default:
        printf("Unexpected character: %c\n", c);
        exit(1);
    }
  }

  scores = insert_ordered(scores, score);
  return scores;
}

int count_scores(ScoreReg* scores) {
  int count = 0;
  for(; scores != NULL; scores=scores->next) {
    //printf("%2.i :: %llu\n", count+1, scores->score);
    count++;
  }
  return count;
}

unsigned long long choose_middle_score(ScoreReg* scores) {
  int middle = count_scores(scores)/2 + 1;
  int i;
  for (i=1; i<middle; i++)
    scores=scores->next;
  return scores->score;
}
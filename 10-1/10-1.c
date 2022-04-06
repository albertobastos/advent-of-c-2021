#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE 1024

int get_syntax_error_score(char*);

int main(int argc, char** args) {
  FILE *fp = NULL;
  char line[1024] = {0};
  size_t len;
  int score = 0;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  while (fgets(line, MAX_LINE, fp))
    score += get_syntax_error_score(line);

  printf("Answer: %d\n", score);

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

int get_syntax_error_score(char* in) {
  Stack st;
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
        if (*in == ')' && c != '(') return 3;
        if (*in == ']' && c != '[') return 57;
        if (*in == '}' && c != '{') return 1197;
        if (*in == '>' && c != '<') return 25137;
        break;
      default:
        break;
    }
  }
  return 0;
}
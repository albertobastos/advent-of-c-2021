#include <stdio.h>
#include <stdlib.h>

#define MAX_ACTION_LENGTH 7 // forward = 7 chars

typedef struct {
  int position;
  int depth;
  int aim;
} State;

int read_line(FILE*, char*, int*);
void apply_action(char*, int, State*);

int main(int argc, char** args) {
  FILE * fp;

  char action[MAX_ACTION_LENGTH+1];
  int amount;
  State state = {0,0,0};

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  while ((read_line(fp, action, &amount)) != EOF) {
    apply_action(action, amount, &state);
  }

  printf("Answer: %d\n", state.position * state.depth);

  fclose(fp);
  exit(0);
}

int read_line(FILE *in, char *action, int *amount) {
  static char *fmt = "%s %d\n";
  return fscanf(in, fmt, action, amount);
}

void apply_action(char* action, int amount, State* state) {
  // no action validation, assumed (f)orward, (u)p, (d)own.
  switch(action[0]) {
    case 'f':
      state->position += amount;
      state->depth += state->aim * amount;
      break;
    case 'u':
      state->aim -= amount;
      break;
    case 'd':
      state->aim += amount;
      break;
    default:
      fprintf(stderr, "Unexpected action: %s\n", action);
      exit(1);
  }
}
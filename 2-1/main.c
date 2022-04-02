#include <stdio.h>
#include <stdlib.h>

#define MAX_ACTION_LENGTH 7 // forward = 7 chars

int read_line(FILE*, char*, int*);
void apply_action(char*, int, int*, int*);

int main(int argc, char** args) {
  FILE * fp;

  char action[MAX_ACTION_LENGTH+1];
  int amount;
  int position = 0;
  int depth = 0;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  while ((read_line(fp, action, &amount)) != EOF) {
    apply_action(action, amount, &position, &depth);
  }

  printf("Answer: %d\n", position*depth);

  fclose(fp);
  exit(0);
}

int read_line(FILE *in, char *action, int *amount) {
  static char *fmt = "%s %d\n";
  return fscanf(in, fmt, action, amount);
}

void apply_action(char* action, int amount, int* position, int* depth) {
  // no action validation, assumed (f)orward, (u)p, (d)own.
  switch(action[0]) {
    case 'f':
      (*position) += amount;
      break;
    case 'u':
      (*depth) -= amount;
      break;
    case 'd':
      (*depth) += amount;
      break;
    default:
      fprintf(stderr, "Unexpected action: %s\n", action);
      exit(1);
  }
}
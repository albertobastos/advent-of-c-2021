#include <stdio.h>
#include <stdlib.h>

#define WINDOW_SIZE 3
#define ACC_SIZE WINDOW_SIZE+1
#define EMPTY -1

typedef int ACC[ACC_SIZE];

void init_acc(ACC);
int update_acc(ACC, int);
int sum_window(ACC, int);
void print_acc(ACC);
  
int main(int argc, char** args) {
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;

  ACC acc;
  int bumps = 0;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  init_acc(acc);
  while ((read = getline(&line, &len, fp)) != EOF) {
    if (update_acc(acc, atoi(line)) != 0) {
      bumps += (sum_window(acc, 0) < sum_window(acc, 1));
    }
  }

  printf("Answer: %d\n", bumps);

  fclose(fp);
  exit(0);
}

void init_acc(ACC acc) {
  int i;
  for (i=0; i<ACC_SIZE; i++)
    acc[i] = EMPTY;
}

// return 1 if the accumulator is filled
int update_acc(ACC acc, int val) {
  int i;
  for (i = 0; i < ACC_SIZE-1; i++) {
    acc[i] = acc[i+1];
  }
  acc[ACC_SIZE-1] = val;
  return acc[0] != EMPTY;
}

int sum_window(ACC acc, int window) {
  int sum = 0;
  int i = 0;
  for (i = window; i < window + WINDOW_SIZE; i++) {
    sum += acc[i];
  }
  return sum;
}

void print_acc(ACC acc) {
  int i = 0;
  printf("ACC: [");
  for (i=0;i<ACC_SIZE;i++)
    printf("%d%s", acc[i], i < ACC_SIZE-1 ? ", " : "");
  printf("]\n");
}
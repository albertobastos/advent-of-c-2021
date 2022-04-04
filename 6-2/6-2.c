#include <stdio.h>
#include <stdlib.h>

#define MAX_TIMER 9
#define DAYS 256

typedef unsigned long long int timer;

void initial_state(FILE*, timer*, timer*);
void step(timer*, timer*);
void print_state(int, timer*, timer);

int main(int argc, char** args) {
  FILE *fp = NULL;
  timer steps[MAX_TIMER];
  timer count;
  int i;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  initial_state(fp, steps, &count);
  for (i=0; i<DAYS; i++) {
    //print_state(i, steps, count);
    step(steps, &count);
  }

  printf("Answer: %llu\n", count);

  fclose(fp);
  exit(0);
}

void initial_state(FILE* in, timer steps[], timer* count) {
  int i;
  for (i=0; i<MAX_TIMER; i++)
    steps[i] = 0;

  *count = 0ULL;
  while (fscanf(in, "%d,", &i) > 0) {
    steps[i]++;
    (*count)++;
  }
}

void step(timer steps[], timer* count) {
  int i;
  timer inc = steps[0];
  (*count) += inc;
  for (i=1;i<MAX_TIMER;i++)
    steps[i-1] = steps[i];
  steps[6] += inc;
  steps[8] = inc;
}

void print_state(int days, timer steps[], timer count) {
  int i;
  printf("<%d> Count: %llu, Steps: [", days, count);
  for (i=0; i<MAX_TIMER; i++)
    printf(" %llu ", steps[i]);
  printf("]\n");
}
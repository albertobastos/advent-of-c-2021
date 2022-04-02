#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** args) {
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  int bumps = 0;
  int prev = -1;
  int curr = -1;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  while ((read = getline(&line, &len, fp)) != EOF) {
    curr = atoi(line);
    if (prev != -1 && curr > prev) {
      bumps++;
    }
    prev = curr;
  }

  printf("Answer: %d\n", bumps);

  fclose(fp);
  exit(0);
}

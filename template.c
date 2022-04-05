#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** args) {
  FILE *fp = NULL;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  printf("Answer: %d\n", -1);

  fclose(fp);
  exit(0);
}

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct TargetArea {
  int xmin;
  int xmax;
  int ymin;
  int ymax;
};
typedef struct TargetArea TargetArea;

TargetArea* read_file(FILE* in) {
  TargetArea* t = malloc(sizeof(TargetArea));
  fscanf(
    in,
    "target area: x=%d..%d, y=%d..%d",
    &(t->xmin), &(t->xmax), &(t->ymin), &(t->ymax)
  );
  return t;
}

int answer_part1(TargetArea* area) {
  return abs(area->ymin) * (abs(area->ymin) - 1) / 2;
}

int main(int argc, char** args) {
  FILE *fp = NULL;
  TargetArea* area = NULL;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  //args[1] = "17-1/sample.in";
  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  area = read_file(fp);

  printf("Answer: %d\n", answer_part1(area));

  fclose(fp);
  exit(0);
}

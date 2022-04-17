#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PROBE_HIT 1
#define PROBE_MISS_X 2
#define PROBE_MISS_Y 3
#define PROBE_MISS_BOTH 4

typedef unsigned int ProbeResult;

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

int hit(TargetArea* area, int x, int y) {
  return x >= area->xmin && x <= area->xmax
    && y >= area->ymin && y <= area->ymax;
}

int miss(TargetArea* area, int x, int y) {
  return x > area->xmax || y < area->ymin;
}

int answer_part2(TargetArea* area) {
  int count = 0;
  int maxheight = abs(area->ymin) * (abs(area->ymin) - 1) / 2;
  int vx;
  int vy;
  int x;
  int y;

  for (int ix=1; ix<=area->xmax; ix++) {
    for (int iy=area->ymin; iy<=maxheight; iy++) {
      vx = ix;
      vy = iy;
      x = 0;
      y = 0;
      //printf("Attempt: %d,%d\n", vx, vy);

      while (1) {
        x += vx;
        y += vy;
        if (vx) vx--;
        vy--;

        //printf("pos: %d,%d [hit: %d] [miss: %d]\n", x, y, hit(area, x, y), miss(area, x, y));
        
        if (hit(area, x, y)) {
          count++;
          break;
        } else if (miss(area, x, y)) {
          break;
        }
      }
    }
  }

  return count;
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

  printf("Answer: %d\n", answer_part2(area));

  fclose(fp);
  exit(0);
}

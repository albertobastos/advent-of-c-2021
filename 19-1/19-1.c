#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BEACONS_PER_SCANNER 30
#define MAX_SCANNERS 30

struct Beacon {
  int x;
  int y;
  int z;
};
typedef struct Beacon Beacon;

struct Scanner {
  Beacon** beacons;
};
typedef struct Scanner Scanner;


void** init_pointer_array(int length) {
  void** arr = malloc(sizeof(void*)*length);
  for (int i=0; i<length; i++) arr[i] = NULL;
  return arr;
}

Scanner** read_file(FILE* in) {
  Scanner** scanners = (Scanner**) init_pointer_array(MAX_SCANNERS);
  Scanner* scanner = NULL;
  Beacon* beacon = NULL;

  int is = 0;
  int ib;

  char* line;
  ssize_t len = 0;
  char* xyz;

  while (getline(&line, &len, in) >= 0) {
    if (strlen(line) == 1) continue; // skip line break
    else if (line[1] == '-') { // new scanner
      scanner = malloc(sizeof(Scanner));
      scanner->beacons = (Beacon**) init_pointer_array(MAX_BEACONS_PER_SCANNER);
      scanners[is++] = scanner;
      ib = 0;
    }
    else { // new beacon
      beacon = malloc(sizeof(Beacon));
      xyz = strtok(line, ",");
      beacon->x = atoi(xyz);
      xyz = strtok(NULL, ",");
      beacon->y = atoi(xyz);
      xyz = strtok(NULL, ",");
      beacon->z = atoi(xyz);
      scanner->beacons[ib++] = beacon;
    }
  }

  return scanners;
}

void print_scanners(Scanner** arr) {
  Scanner* s = NULL;
  Beacon* b = NULL;
  for (int i=0; arr[i] != NULL; i++) {
    printf("--- scanner %d ---\n", i);
    s = arr[i];
    for (int j=0; s->beacons[j] != NULL; j++) {
      b = s->beacons[j];
      printf("%d,%d,%d\n", b->x, b->y, b->z);
    }
    printf("\n");
  }
}

int main(int argc, char** args) {
  FILE *fp = NULL;
  Scanner** scanners = NULL; 

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  args[1] = "19-1/sample.in";
  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  scanners = read_file(fp);

  print_scanners(scanners);

  printf("Answer: %d\n", -1);

  fclose(fp);
  exit(0);
}

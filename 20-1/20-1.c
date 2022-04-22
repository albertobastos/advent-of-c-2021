#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KEY_LENGTH 512
#define STEPS 2

int** read_file(FILE* in, int* alg, int* size) {
  int** img = NULL;
  char buf[KEY_LENGTH+1];
  int line;

  // read image enhancement algorithm
  fgets(buf, KEY_LENGTH+1, in);
  for (int i=0; i<KEY_LENGTH; i++)
    alg[i] = buf[i] == '#';

  // skip blank lines  
  do {
    fgets(buf, KEY_LENGTH+1, in);
  } while(buf[0] == '\n');

  // read initial image, first line determines initial size
  *size = -1;
  line = 0;
  do {
    if (*size == -1) {
      *size = strlen(buf)-1;
      img = malloc(sizeof(int*)*(*size));
    }
    
    img[line] = malloc(sizeof(int)*(*size));
    for (int i=0; i < *size; i++)
      img[line][i] = buf[i] == '#';
    line++;
  } while (fgets(buf, KEY_LENGTH+1, in));

  return img;
}

int** do_step(int** grid, int* size, int* alg) {
  int nsize = *size + 2;
  int** ngrid = malloc(sizeof(int*)*nsize);

  for (int i=0; i<nsize; i++) {
    ngrid[i] = malloc(sizeof(int)*nsize);
    for (int j=0; j<nsize; j++) {
      ngrid[i][j] = 0; // TODO: calc based on grid and key
    }
  }

  *size = nsize;
  return ngrid;
}

int count_lit(int** grid, int size) {
  int count = 0;
  for (int i=0; i<size; i++)
    for (int j=0; j<size; j++)
      count += grid[i][j];
  return count;
}

void print_grid(int** grid, int size) {
  if (grid == NULL) {
    printf("NULL grid.\n");
    return;
  }
  for (int i=0; i<size; i++) {
    for (int j=0; j<size; j++) {
      printf("%c", grid[i][j] == 1 ? '#' : '.');
    }
    printf("\n");
  }
}

int main(int argc, char** args) {
  FILE *fp = NULL;
  int alg[KEY_LENGTH];
  int** grid = NULL;
  int grid_size;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  grid = read_file(fp, alg, &grid_size);
  for (int i=1; i<=STEPS; i++) {
    grid = do_step(grid, &grid_size, alg);
    printf("After step %d (size: %d)\n", i, grid_size);
    print_grid(grid, grid_size);
  }

  printf("Answer: %d\n", count_lit(grid, grid_size));

  fclose(fp);
  exit(0);
}

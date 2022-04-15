#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(x, y) (((x) > (y)) ? (y) : (x))

struct State {
  char* buff;
  int bitPos;
  int bitSize;
};
typedef struct State State;

typedef unsigned long Chunk;

State* init_state(FILE* in) {
  char* buff;
  ssize_t len = 0;
  getline(&buff, &len, in);
  len = strlen(buff);
  buff[len] = '\0';

  State* s = malloc(sizeof(State));
  s->buff = buff;
  s->bitPos = 0;
  s->bitSize = strlen(buff)*4;

  return s;
}

int hex2dec(char c) {
  //printf("hex2dec(%c)\n", c);
  if (c >= '0' && c <= '9')
    return c - '0';
  else if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  
  printf("Invalid hexadecimal digit: %c\n", c);
  exit(1);
}

int mask(short bits) {
  switch (bits) {
    case 0: return 0;
    case 1: return 1;
    case 2: return 3;
    case 3: return 7;
    case 4: return 15;
    default:
      printf("Unsupported mask for %d bits.", bits);
      exit(1);
  }
}

Chunk read(State* state, int bits) {
  int remaining = bits;
  Chunk chunk = 0;
  int hexPos;
  int hexAvailable;
  int hexBits;

  if (bits > sizeof(Chunk)) {
    printf("Cannot read more than %d>%ld bits at once.", bits, sizeof(Chunk));
    exit(1);
  }

  if (remaining > (state->bitSize - state->bitPos)) {
    printf("Not enough data to read %d more bits.\n", bits);
    exit(1);
  }

  while (remaining > 0) {
    hexPos = state->bitPos / 4;
    hexAvailable = 4 - (state->bitPos % 4);
    hexBits = MIN(remaining, hexAvailable);
    chunk = (chunk << hexBits) | ((hex2dec(state->buff[hexPos]) & mask(hexAvailable)) >> (hexAvailable - hexBits));
    remaining -= hexBits;
    state->bitPos += hexBits;
  }
  return chunk;
}

int main(int argc, char** args) {
  FILE *fp = NULL;
  State* state = NULL;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  //args[1] = "16-1/sample.in";
  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  state = init_state(fp);

  while (state->bitPos < state->bitSize) {
    //printf("%lu", read(state, 4));
    //printf("%x", read(state, 4));
    read(state, 1);
  }
  printf("\n");

  printf("Answer: %d\n", -1);

  fclose(fp);
  exit(0);
}
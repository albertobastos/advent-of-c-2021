#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MIN(x, y) (((x) > (y)) ? (y) : (x))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#define DEBUG 0

struct State {
  char* buff;
  int bitPos;
  int bitSize;
  int sum_versions;
};
typedef struct State State;

typedef unsigned long Chunk;
int ChunkBitsize = sizeof(Chunk)*8;
Chunk ChunkMax = ULONG_MAX;

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

  if (bits > ChunkBitsize) {
    printf("Cannot read more than %d>%d bits at once.", bits, ChunkBitsize);
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

  //printf("read %d = %lu\n", bits, chunk);
  return chunk;
}

int remaining(State* state) {
  return state->bitSize - state->bitPos;
}

Chunk process(State*);
Chunk process_packet(State*);
Chunk process_literal(State*);
Chunk process_operator(State*, Chunk);

Chunk process_literal(State* state) {
  if (DEBUG) printf("process_literal at %d... ", state->bitPos);
  Chunk l = 0;
  int groupBit;
  do {
    groupBit = read(state, 1);
    l = (l << 4) | read(state, 4);
  } while(groupBit == 1);
  return l;
}

Chunk process_operator_sum(State* state) {
  Chunk sum = 0;
  Chunk curr;
  Chunk typeID = read(state, 1);
  switch (typeID) {
    case 0:
      int targetPos = state->bitPos + 15 + read(state, 15);
      while (state->bitPos < targetPos) {
        curr = process_packet(state);
        sum += curr;
      }
      break;
    case 1:
      for (Chunk packets = read(state, 11); packets > 0; packets--) {
        curr = process_packet(state);
        sum += curr;
      }
      break;
    default:
      printf("Unsupported type ID %lu\n", typeID);
      exit(1);
  }
  return sum;
}

Chunk process_operator_prod(State* state) {
  Chunk prod = 1;
  Chunk curr;
  Chunk typeID = read(state, 1);
  switch (typeID) {
    case 0:
      int targetPos = state->bitPos + 15 + read(state, 15);
      while (state->bitPos < targetPos) {
        curr = process_packet(state);
        prod *= curr;
      }
      break;
    case 1:
      for (Chunk packets = read(state, 11); packets > 0; packets--) {
        curr = process_packet(state);
        prod *= curr;
      }
      break;
    default:
      printf("Unsupported type ID %lu\n", typeID);
      exit(1);
  }
  return prod;
}

Chunk process_operator_min(State* state) {
  Chunk min = ChunkMax;
  Chunk curr;
  Chunk typeID = read(state, 1);
  switch (typeID) {
    case 0:
      int targetPos = state->bitPos + 15 + read(state, 15);
      while (state->bitPos < targetPos) {
        curr = process_packet(state);
        min = MIN(min, curr);
      }
      break;
    case 1:
      for (Chunk packets = read(state, 11); packets > 0; packets--) {
        curr = process_packet(state);
        min = MIN(min, curr);
      }
      break;
    default:
      printf("Unsupported type ID %lu\n", typeID);
      exit(1);
  }
  return min;
}

Chunk process_operator_max(State* state) {
  Chunk max = 0;
  Chunk curr;
  Chunk typeID = read(state, 1);
  switch (typeID) {
    case 0:
      int targetPos = state->bitPos + 15 + read(state, 15);
      while (state->bitPos < targetPos) {
        curr = process_packet(state);
        max = MAX(max, curr);
      }
      break;
    case 1:
      for (Chunk packets = read(state, 11); packets > 0; packets--) {
        curr = process_packet(state);
        max = MAX(max, curr);
      }
      break;
    default:
      printf("Unsupported type ID %lu\n", typeID);
      exit(1);
  }
  return max;
}

void skip_typeIDlength(State* state) {
  Chunk typeID = read(state, 1);
  switch (typeID) {
    case 0: read(state, 15); break;
    case 1: read(state, 11); break;
    default:
      printf("Unsupported type ID %lu\n", typeID);
      exit(1);
  }  
}

Chunk process_operator_gt(State* state) {
  skip_typeIDlength(state);
  return process_packet(state) > process_packet(state) ? 1 : 0;
}

Chunk process_operator_lt(State* state) {
  skip_typeIDlength(state);
  return process_packet(state) < process_packet(state) ? 1 : 0;
}

Chunk process_operator_eq(State* state) {
  skip_typeIDlength(state);
  return process_packet(state) == process_packet(state) ? 1 : 0;
}

Chunk process_operator(State* state, Chunk op) {
  if (DEBUG) printf("process_operator %lu at %d\n", op, state->bitPos);
  switch (op) {
    case 0: return process_operator_sum(state);
    case 1: return process_operator_prod(state);
    case 2: return process_operator_min(state);
    case 3: return process_operator_max(state);
    case 5: return process_operator_gt(state);
    case 6: return process_operator_lt(state);
    case 7: return process_operator_eq(state);
    default:
      printf("Unsupported opcode %lu\n", op);
      exit(1);
  }
}

Chunk process_packet(State* state) {
  if (DEBUG) printf("process_packet at %d\n", state->bitPos);
  state->sum_versions += read(state, 3);
  Chunk op = read(state, 3);
  switch (op) {
    case 4:
      return process_literal(state);
    default:
      return process_operator(state, op);
  }
}

Chunk process(State* state) {
  state->sum_versions = 0;
  return process_packet(state);
}

int main(int argc, char** args) {
  FILE *fp = NULL;
  State* state = NULL;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  //args[1] = "16-2/sample.in";
  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  state = init_state(fp);

  Chunk result = process(state);
  printf("Answer: %lu\n", result);

  fclose(fp);
  exit(0);
}
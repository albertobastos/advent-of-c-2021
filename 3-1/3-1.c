#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINEFMT "%s\n"

int calc_bits(FILE*);
unsigned int create_mask(int);

int main(int argc, char** args) {
  FILE * fp;
  unsigned int line;
  int bits;
  int i;
  int lines = 0;
  unsigned int mask;
  unsigned int gamma;
  unsigned int epsilon;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  bits = calc_bits(fp);
  mask = create_mask(bits);
  int ones[bits];
  char buf[bits+1];

  for (i = 0; i < bits ; i++)
    ones[i] = 0;

  while (fscanf(fp, LINEFMT, buf) != EOF) {
    for (i = 0; i < bits; i++)
      if (buf[i] == '1') ones[i]++;
    lines++;
  }

  gamma = 0;
  for (i = 0; i < bits; i++) {
    if (ones[i] > lines/2) {
      gamma |= 0x01 << (bits-i-1);
    }
  }
  epsilon = ~gamma & mask;

  printf("Answer: %ld\n", (unsigned long) gamma * epsilon);

  fclose(fp);
  exit(0);
}

int calc_bits(FILE *in) {
  char buf[16];
  int bits;
  if (fscanf(in, LINEFMT, buf) == EOF) return 0;
  bits = strlen(buf);
  fseek(in, 0, SEEK_SET);
  return bits;
}

unsigned int create_mask(int bits) {
  unsigned int mask = 0;
  for (; bits-- > 0;) {
    mask <<= 1;
    mask |= 0x01;
  }
  return mask;
}

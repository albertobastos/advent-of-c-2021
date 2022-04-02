#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NUM_LINES 1024
#define LINE_LENGTH 13

#define OXYGEN 1
#define CO2 0

char* choose(char**, int, int);
unsigned int binary_to_uint(const char*);

int main(int argc, char** args) {
  FILE *fp;
  char *buf;
  char *lines[MAX_NUM_LINES];
  size_t len;
  int n = 0;
  int i;
  int read;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  while ((read = getline(&buf, &len, fp)) > 1) {
    buf[read-1] = '\0';
    lines[n++] = strdup(buf);
  }

  while (n < MAX_NUM_LINES)
    lines[n++] = NULL;

  /*
  for (i=0; i<MAX_NUM_LINES; i++)
    printf("#%d -- %s\n", i, lines[i]);
  */

  unsigned int oxygen = binary_to_uint(choose(lines, OXYGEN, 0));
  unsigned int co2 = binary_to_uint(choose(lines, CO2, 0));

  if (oxygen == 0) {
    printf("Oxygen generator rating cannot be determined.");
    exit(1);
  }
  if (co2 == 0) {
    printf("CO2 scrubber rating cannot be determined.");
    exit(1);
  }


  printf("Answer: %d\n", oxygen*co2);

  fclose(fp);
  exit(0);
}

char* choose(char** lines, int type, int pos) {
  int n = 0;
  int i;
  int ones = 0;
  char target;

  for (i=0; i<MAX_NUM_LINES; i++) {
    if (lines[i] != NULL) {
      if (lines[i][pos] == '1') ones++;
      n++;
    }
  }

  if (n == 0) return NULL;

  if (ones >= n/2) {
    target = type == OXYGEN ? '1' : '0';
  } else if (ones < n/2) {
    target = type == OXYGEN ? '0' : '1';
  }

  char* newlines[MAX_NUM_LINES];
  n = 0;
  char* last = NULL;

  for (i=0; i<MAX_NUM_LINES; i++) {
    newlines[i] = NULL;
    if (lines[i] != NULL) {
      if (lines[i][pos] == target) {
        newlines[i] = lines[i];
        last = lines[i];
        n++;
      }
    }
  }

  if (n > 1) return choose(newlines, type, pos+1);
  return last;
}

unsigned int binary_to_uint(const char *b)
{
  unsigned int val = 0;
  int i = 0;

  if (b == NULL)
    return 0;

  while (b[i] == '0' || b[i] == '1')
  {  // Found another digit.
    val <<= 1;
    val += b[i]-'0';
    i++;
  }
  return val;
}
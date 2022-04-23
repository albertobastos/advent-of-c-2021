// struggled with how to find a non-brute force solution
// inspired (almost copied) from this awesome solution:
// https://www.reddit.com/r/adventofcode/comments/rl6p8y/comment/hpkxh2c/?utm_source=reddit&utm_medium=web2x&context=3

#include <stdio.h>
#include <stdlib.h>

#define MAX(a,b) (a > b) ? a : b;
#define GOAL 21
#define POSITIONS 10
#define VARIANTS 7

int ROLL_VALUES[VARIANTS] =    {3, 4, 5, 6, 7, 8, 9};
int ROLL_UNIVERSES[VARIANTS] = {1, 3, 6, 7, 6, 3, 1};

void read_file(FILE* in, int* p1pos, int* p2pos) {
  int dummy;
  fscanf(in, "Player %d starting position: %d\n", &dummy, p1pos);
  fscanf(in, "Player %d starting position: %d", &dummy, p2pos);
}

void fill_wins(int p1pos, int p1score, long* p1wins, int p2pos, int p2score, long* p2wins) {
  *p1wins = 0;
  *p2wins = 0;
  if (p2score >= GOAL) {
    (*p2wins)++;
    return;
  }

  long rp1wins;
  long rp2wins;
  int val;
  int univs;
  int newpos;
  long newscore;
  for (int v=0; v<VARIANTS; v++) {
    int val = ROLL_VALUES[v];
    int univs = ROLL_UNIVERSES[v];
    // is always player 1 turn as we flip on each recursive call
    newpos = p1pos + val;
    if (newpos > POSITIONS) newpos -= POSITIONS;
    newscore = p1score + newpos; 
    fill_wins(p2pos, p2score, &rp2wins, newpos, newscore, &rp1wins);
    *p1wins = *p1wins + univs*rp1wins;
    *p2wins = *p2wins + univs*rp2wins;
  }
}

long calc_part2(int p1init, int p2init) {
  long p1wins = 0;
  long p2wins = 0;
  fill_wins(p1init, 0, &p1wins, p2init, 0, &p2wins);
  return MAX(p1wins, p2wins);
}

int main(int argc, char** args) {
  FILE *fp = NULL;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  //args[1] = "21-2/sample.in";
  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  int p1pos;
  int p2pos;

  read_file(fp, &p1pos, &p2pos);

  printf("Answer: %ld\n", calc_part2(p1pos, p2pos));

  fclose(fp);
  exit(0);
}

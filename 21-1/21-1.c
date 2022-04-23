#include <stdio.h>
#include <stdlib.h>

#define MIN(a,b) (a > b) ? b : a;
#define GOAL 1000
#define POSITIONS 10

struct Player {
  int pos;
  int score;
};
typedef struct Player Player;

struct State {
  struct Player* players[2];
  int dice;
  int rolls;
};
typedef struct State State;

State* read_file(FILE* in) {
  State* state = malloc(sizeof(State));
  state->dice = 1;
  state->rolls = 0;
  state->players[0] = malloc(sizeof(Player));
  state->players[1] = malloc(sizeof(Player));
  state->players[0]->score = 0;
  state->players[1]->score = 0;
  int dummy;
  fscanf(in, "Player %d starting position: %d\n", &dummy, &(state->players[0]->pos));
  fscanf(in, "Player %d starting position: %d", &dummy, &(state->players[1]->pos));
  return state;
}

int pick_player(State* game) {
  return game->rolls % 6 == 0 ? 0 : 1;
}

int roll_dice(State* game, int rolls) {
  int sum = 0;
  for (int i=0; i<rolls; i++) {
    sum += (game->dice++);
    if (game->dice > 100) game->dice = 1; 
  }
  game->rolls += rolls;
  return sum;
}

void play(State* game) {
  int pi;
  Player* p;
  int move;
  while (game->players[0]->score < GOAL && game->players[1]->score < GOAL) {
    pi = pick_player(game);
    p = game->players[pi];
    move = roll_dice(game, 3);
    p->pos += move;
    while (p->pos > 10) p->pos -= 10;
    p->score += p->pos;
  }
}

int main(int argc, char** args) {
  FILE *fp = NULL;
  State* game = NULL;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  //args[1] = "21-1/sample.in";
  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  game = read_file(fp);

  play(game);

  int loser = MIN(game->players[0]->score, game->players[1]->score);
  int answer = loser * game->rolls;

  printf("Answer: %d\n", answer);

  fclose(fp);
  exit(0);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMBERS 100
#define PUNCHED -1
#define NCOLS 5
#define NROWS 5

struct Card {
  int items[NCOLS*NROWS];
};
typedef struct Card Card;

struct CardNode {
  struct Card *card;
  struct CardNode *next;
};
typedef struct CardNode CardNode;

void *read_numbers(FILE*, int*);
CardNode *read_cards(FILE*);
Card *punch_cards(CardNode*, int);
int check_winner(Card*);
int get_item(Card*, int, int);
int winner_score(Card*, int);
void print_card(Card*);

int main(int argc, char** args) {
  FILE *fp;
  int numbers[NUMBERS];
  CardNode *first_card;
  Card *winner;
  int i;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  read_numbers(fp, numbers);
  first_card = read_cards(fp);

  winner = NULL;
  for (i=0;i<NUMBERS && winner == NULL;i++) {
    winner = punch_cards(first_card, numbers[i]);
  }

  if (winner == NULL) {
    printf("Answer: no winners!\n");
    exit(1);
  }

  printf("Answer: %d\n", winner_score(winner, numbers[i-1]));

  fclose(fp);
  exit(0);
}

void *read_numbers(FILE* in, int* numbers) {
  int n;
  int *next = numbers;
  for (n = 0; n < NUMBERS; n++) {
    fscanf(in, "%d,", next++);
  }

  return numbers;
}

CardNode *read_cards(FILE* in) {
  CardNode *first = NULL;
  CardNode *curr = NULL;
  CardNode *tmp = NULL;
  int val;
  int n = 0;
  while (fscanf(in, "%d ", &val) > 0) {
    if (n == 0) {
      // new card
      if (first == NULL) {
        first = curr = malloc(sizeof(CardNode));
      } else {
        tmp = curr;
        curr = malloc(sizeof(CardNode));
        tmp->next = curr;
      }
      curr->card = malloc(sizeof(Card));
      curr->next = NULL;
    }
    curr->card->items[n] = val;
    n = ++n % (NCOLS*NROWS);
  }
  return first;
}

Card *punch_cards(CardNode* list, int val) {
  Card *c;
  int i;
  while (list != NULL) {
    c = list->card;
    for (i=0; i<NCOLS*NROWS; i++) {
      if (c->items[i] == val) {
        c->items[i] = PUNCHED;
        if (check_winner(c)) return c;
        else i = NCOLS*NROWS; // break items loop
      }
    }
    list = list->next;
  }
}

int check_winner(Card* card) {
  int row, col;
  for (row=0;row<NROWS;row++) {
    for (col=0;col<NCOLS;col++) {
      if (get_item(card, row, col) != PUNCHED)
        break;
    }
    if (col == NCOLS) // all row punched
      return 1;
  }
  for (col=0;col<NCOLS;col++) {
    for (row=0;row<NROWS;row++) {
      if (get_item(card, row, col) != PUNCHED)
        break;
    }
    if (row == NROWS) // all col punched
      return 1;
  }
  return 0;
}

int get_item(Card* card, int row, int col) {
  return card->items[row*NCOLS+col];
}

int winner_score(Card* card, int lastCalled) {
  int sum = 0;
  int i;
  for (i=0;i<NCOLS*NROWS;i++)
    sum += card->items[i] != PUNCHED ? card->items[i] : 0;
  return sum*lastCalled;
}

void print_card(Card* card) {
  printf("Card:\n");
  int row, col;
  for (row=0;row<NROWS;row++) {
    for (col=0;col<NCOLS;col++) {
      printf("%3d ", get_item(card, row, col));
    }
    printf("\n");
  }
  printf("\n");
}
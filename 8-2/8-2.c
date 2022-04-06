#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
  0   abcefg    6 tracks
  1   cf        2 tracks
  2   acdeg     5 tracks
  3   acdfg     5 tracks
  4   bcdf      4 tracks
  5   abdfg     5 tracks
  6   abdefg    6 tracks
  7   acf       3 tracks
  8   abcdefg   7 tracks
  9   abcdfg    6 tracks
*/

#define PATTERNS 10
#define DISPLAYS 4
#define TRACKS 7

struct Reading {
  struct Reading *next;
  char* patterns[PATTERNS];
  char* displays[DISPLAYS];
  int decoded;
};
typedef struct Reading Reading;

Reading *read_file(FILE*);
unsigned long tracks(char*);
void decode_readings(Reading*);
void do_decode(Reading*);

int main(int argc, char** args) {
  FILE *fp = NULL;
  Reading *head = NULL;
  int key[TRACKS]; // ['a'-'a'], ['b'-'a'], ... ['g'-'a']

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  head = read_file(fp);
  decode_readings(head);

  int sum = 0;
  while (head != NULL) {
    sum += head->decoded;
    head = head->next;
  }

  printf("Answer: %d\n", sum);

  fclose(fp);
  exit(0);
}

Reading *read_file(FILE* in) {
  Reading* head = NULL;
  Reading* curr = NULL;
  char buf[TRACKS+1];
  int i;

  while (!feof(in)) {
    if (head == NULL) {
      head = malloc(sizeof(Reading));
      curr = head;
    } else {
      curr->next = malloc(sizeof(Reading));
      curr = curr->next;
    }

    for (i = 0; i<PATTERNS; i++) {
      fscanf(in, "%s", buf);
      curr->patterns[i] = strdup(buf);
    }
    fscanf(in, "%s", buf);
    if (strcmp(buf, "|") != 0) {
      printf("Error: pipe (|) expected.\n");
      exit(1);
    }
    for (i = 0; i<DISPLAYS; i++) {
      fscanf(in, "%s", buf);
      curr->displays[i] = strdup(buf);
    }    
  }

  return head;
}

unsigned long tracks(char* in) {
  return strlen(in);
}

void decode_readings(Reading* curr) {
  if (curr == NULL) return;
  do_decode(curr);
  decode_readings(curr->next);
}

typedef char Key[TRACKS];

void init_key(Key key) {
  int i;
  for (i=0; i<TRACKS; i++)
    key[i] = '_';
}

void print_key(Key key) {
  int i;
  printf("Key: ");
  for (i=0; i<TRACKS; i++)
    printf("%c=%c ", 'a'+i, key[i]);
  printf("\n");
}

char *strdiff(char* target, char* stra, char* strb) {
  int added = 0;
  char* tmp;
  int found;
  for (; *stra; *stra++) {
    tmp = strb;
    found = 0;
    for (; *tmp && !found; *tmp++) {
      if (*tmp == *stra) found = 1;
    }
    if (!found) target[added++] = *stra;
  }
  target[added++] = '\0';
}

char *strcommon(char* target, char* stra, char* strb) {
  int added = 0;
  char* tmp;
  int found;
  for (; *stra; *stra++) {
    tmp = strb;
    found = 0;
    for (; *tmp && !found; *tmp++) {
      if (*tmp == *stra) found = 1;
    }
    if (found) target[added++] = *stra;
  }
  target[added++] = '\0';
}

char encode(Key key, char c) {
  int i;
  for (i=0; i<TRACKS; i++)
    if (key[i] == c) return 'a'+i;
  return '_';
}

char decode(Key key, char c) {
  return key[c-'a'];
}

char buff[TRACKS+1];
char buff2[TRACKS+1];

// Keys already known: -
// The difference between the 3-track and the 2-track pattern is the "a" key
void guess_a(char** patterns, Key key) {
  char* p2t = NULL;
  char* p3t = NULL;

  for (; *patterns && (p2t == NULL || p3t == NULL); *patterns++) {
    if (strlen(*patterns) == 2)
      p2t = *patterns;
    else if (strlen(*patterns) == 3)
      p3t = *patterns;
  }

  strdiff(buff, p3t, p2t);
  key[buff[0]-'a'] = 'a';
}

// Keys already known: a
// Find a 5-track pattern that, removing the tracks from 1 (the 2-track pattern)
// and the "a" key, only has two tracks remaining.
// The common track between these two and the 4-track digit is the "d" key.
void guess_d(char** patterns, Key key) {
  char* p2t = NULL;
  char* p4t = NULL;
  char** tmp = patterns;
  int found;
  
  for (; *tmp && (p2t == NULL || p4t == NULL); *tmp++) {
    if (strlen(*tmp) == 2)
      p2t = *tmp;
    else if (strlen(*tmp) == 4)
      p4t = *tmp;
  }

  buff[0] = p2t[0];
  buff[1] = p2t[1];
  buff[2] = encode(key, 'a');
  buff[3] = '\0';

  tmp = patterns;
  found = 0;

  for (; *tmp && !found; *tmp++) {
    if (strlen(*tmp) == 5) {
      strdiff(buff2, *tmp, buff);
      if (strlen(buff2) == 2) found = 1;
    }
  }

  strcommon(buff, buff2, p4t);
  key[buff[0]-'a'] = 'd';
}

// Keys already known: a, d
// Find the 4-track pattern and remove the tracks from 1 (the 2-track pattern)
// and the "d" key. The remaining one is b.
void guess_b(char** patterns, Key key) {
  char* p2t = NULL;
  char* p4t = NULL;
  char** tmp = patterns;
  int found;
  
  for (; *tmp && (p2t == NULL || p4t == NULL); *tmp++) {
    if (strlen(*tmp) == 2)
      p2t = *tmp;
    else if (strlen(*tmp) == 4)
      p4t = *tmp;
  }

  buff[0] = p2t[0];
  buff[1] = p2t[1];
  buff[2] = encode(key, 'd');
  buff[3] = '\0';

  strdiff(buff2, p4t, buff);
  key[buff2[0]-'a'] = 'b';
}

// Keys already known: a, b, d
// Find a 5-track pattern that, removing the tracks from 1 (the 2-track pattern)
// and the "a" and "d" keys only has one track remaining. That is g.
void guess_g(char** patterns, Key key) {
  char* p2t = NULL;
  char** tmp = patterns;
  int found;
  
  for (; *tmp && p2t == NULL; *tmp++) {
    if (strlen(*tmp) == 2)
      p2t = *tmp;
  }

  buff[0] = p2t[0];
  buff[1] = p2t[1];
  buff[2] = encode(key, 'a');
  buff[3] = encode(key, 'd');
  buff[4] = '\0';

  tmp = patterns;
  found = 0;

  for (; *tmp && !found; *tmp++) {
    if (strlen(*tmp) == 5) {
      strdiff(buff2, *tmp, buff);
      if (strlen(buff2) == 1) found = 1;
    }
  }

  key[buff2[0]-'a'] = 'g';
}

// Keys already known: a, b, d, g
// Find a 5-track pattern that, removing the keys from "a", "b", "d" and "g",
// only has one track remaining. This is f.
void guess_f(char** patterns, Key key) {
  int found;
  buff[0] = encode(key, 'a');
  buff[1] = encode(key, 'b');
  buff[2] = encode(key, 'd');
  buff[3] = encode(key, 'g');
  buff[4] = '\0';

  found = 0;

  for (; *patterns && !found; *patterns++) {
    if (strlen(*patterns) == 5) {
      strdiff(buff2, *patterns, buff);
      if (strlen(buff2) == 1) found = 1;
    }
  }

  key[buff2[0]-'a'] = 'f';
}

// Keys already known: a, b, d, f, g
// Remove the "f" key from the 2-track pattern, the remaining is "c".
void guess_c(char** patterns, Key key) {
  char* p2t = NULL;
  for (; *patterns && p2t == NULL; *patterns++) {
    if (strlen(*patterns) == 2)
      p2t = *patterns;
  }

  buff[0] = encode(key, 'f');
  buff[1] = '\0';

  strdiff(buff2, p2t, buff);
  key[buff2[0]-'a'] = 'c';
}

// Keys already known: a, b, c, d, f, g
// The remaining unknown key is e
void guess_e(char** patterns, Key key) {
  int i;
  for (i=0; i<TRACKS; i++) {
    if (key[i] == '_') {
      key[i] = 'e';
      return;
    }
  }
}

int include_keys(char* display, char* keys, Key key) {
  char* tmp;
  char ekey;
  int found;

  for (; *keys; *keys++) {
    ekey = encode(key, *keys);
    tmp = display;
    found = 0;
    for (; *tmp && !found; *tmp++) {
      found = (*tmp == ekey);
    }
    if (!found) return 0;
  }

  return 1;
}

int decode_digit(char* display, Key key) {
  switch (strlen(display)) {
    case 2: return 1;
    case 3: return 7;
    case 4: return 4;
    case 7: return 8;
    case 5:
      if (include_keys(display, "ce", key)) return 2;
      else if (include_keys(display, "cf", key)) return 3;
      else return 5;
    case 6:
      if (include_keys(display, "ce", key)) return 0;
      else if (include_keys(display, "de", key)) return 6;
      else return 9;
  }
}

void do_decode(Reading* r) {
  Key key;
  init_key(key);

  guess_a(r->patterns, key);
  guess_d(r->patterns, key);
  guess_b(r->patterns, key);
  guess_g(r->patterns, key);
  guess_f(r->patterns, key);
  guess_c(r->patterns, key);
  guess_e(r->patterns, key);

  int n = 0;
  int i;
  int digit;
  for (i=0; i<DISPLAYS; i++) {
    digit = decode_digit(r->displays[i], key);
    n = (n*10) + digit;
  }

  r->decoded = n;
}

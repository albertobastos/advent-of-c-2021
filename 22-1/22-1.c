#include <stdio.h>
#include <stdlib.h>

// Apply set theory:
// Union(A,B) = A + B - Intersection(A,B)
// Have positive and negative cubes
// For each positive cube:
// - Add it as positive
// - For each intersection with an existing POSITIVE cube, add it as negative
// For each negative cube:
// - For each intersection with an existing POSITIVE cube, add it as negative

#define MIN(a,b) (a < b) ? a : b;
#define MAX(a,b) (a > b) ? a : b;

struct Cuboid {
  int sign;
  int xfrom;
  int xto;
  int yfrom;
  int yto;
  int zfrom;
  int zto;
};
typedef struct Cuboid Cuboid;

struct CuboidList {
  struct Cuboid* elem;
  struct CuboidList* next;
};
typedef struct CuboidList CuboidList;

CuboidList* append(CuboidList* list, Cuboid* elem) {
  CuboidList *head = list;
  if (head == NULL) {
    list = malloc(sizeof(CuboidList));
    list->elem = elem;
    list->next = NULL;
    head = list;
  } else {
    for (; list->next != NULL; list=list->next) {}
    list->next = malloc(sizeof(CuboidList));
    list->next->elem = elem;
    list->next->next = NULL;
  }
  return head;
}

CuboidList* append_all(CuboidList* list, CuboidList* add) {
  for (; add != NULL; add=add->next)
    list = append(list, add->elem);
  return list;
}

char should_skip(Cuboid *c) {
  return c->xfrom < -50 || c->xto > 50
    || c->yfrom < -50 || c->yto > 50
    || c->zfrom < -50 || c->zto > 50;
}

CuboidList* read_file(FILE* in) {
  CuboidList* list = NULL;

  Cuboid* c = NULL;
  char onoff[4];
  int xfrom, xto, yfrom, yto, zfrom, zto;
  while ((fscanf(
    in, "%s x=%d..%d,y=%d..%d,z=%d..%d\n",
    onoff, &xfrom, &xto, &yfrom, &yto, &zfrom, &zto) == 7 )
  ) {
    c = malloc(sizeof(Cuboid));
    c->sign = onoff[1] == 'n' ? 1 : -1;
    c->xfrom = xfrom;
    c->xto = xto;
    c->yfrom = yfrom;
    c->yto = yto;
    c->zfrom = zfrom;
    c->zto = zto;
    
    if (!should_skip(c)) list = append(list, c);
  }
  return list;
}

Cuboid* intersection(Cuboid* a, Cuboid* b) {
  Cuboid *c = malloc(sizeof(Cuboid));
  c->sign = -1*(b->sign);
  c->xfrom = MAX(a->xfrom, b->xfrom);
  c->xto = MIN(a->xto, b->xto);
  c->yfrom = MAX(a->yfrom, b->yfrom);
  c->yto = MIN(a->yto, b->yto);
  c->zfrom = MAX(a->zfrom, b->zfrom);
  c->zto = MIN(a->zto, b->zto);

  if (c->xfrom > c->xto || c->yfrom > c->yto || c->zfrom > c->zto) {
    free(c);
    return NULL;
  }

  return c;
}

int cuboid_size(Cuboid *c) {
  return
    (c->xto - c->xfrom  + 1)
    * (c->yto - c->yfrom + 1)
    * (c->zto - c->zfrom + 1);
}

void print_cuboid(Cuboid *c) {
  if (c == NULL) {
    printf("Null cuboid\n");
    return;
  }
  printf("%s x=%d..%d,y=%d..%d,z=%d..%d (size: %d)\n",
    c->sign == 1 ? "on" : "off",
    c->xfrom, c->xto, c->yfrom, c->yto, c->zfrom, c->zto,
    cuboid_size(c));
}

void print_list(CuboidList* list) {
  Cuboid* c = NULL;
  for (; list != NULL; list=list->next)
    print_cuboid(list->elem);
}

CuboidList* process(CuboidList* list) {
  CuboidList *plist = NULL;
  CuboidList *to_add = NULL;
  for (; list != NULL; list=list->next) {
    Cuboid *c = list->elem;
    CuboidList *trg = plist;
    to_add = NULL;
    if (c->sign == 1) to_add = append(to_add, c);
    for (; trg != NULL; trg=trg->next) {
      Cuboid *inter = intersection(c, trg->elem);
      if (inter != NULL)
        to_add = append(to_add, inter);
    }
    plist = append_all(plist, to_add);
  }
  return plist;
}

int count_cubes(CuboidList *list) {
  int count = 0;
  Cuboid *c = NULL;
  for (; list != NULL; list=list->next) {
    c = list->elem;
    count += (cuboid_size(c)*c->sign);
  }
  return count;
}

int main(int argc, char** args) {
  FILE *fp = NULL;
  CuboidList* list = NULL;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  //args[1] = "22-1/sample.in";
  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  list = read_file(fp);
  list = process(list);
  //print_list(list);
  
  printf("Answer: %d\n", count_cubes(list));

  fclose(fp);
  exit(0);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_BEACONS_PER_SCANNER 30
#define MAX_SCANNERS 30
#define NUM_ROTATIONS 6*8
#define REQUIRED_PAIRS 66
#define UNSET -1

struct Beacon {
  int coords[3];
};
typedef struct Beacon Beacon;

struct Scanner {
  int index;
  Beacon** beacons;
  int beaconCount;
  double** distances;
  int origin[3];
  int* (*rotation)(int*);
};
typedef struct Scanner Scanner;

int* coords(int x, int y, int z) {
  int* c = malloc(sizeof(int)*3);
  c[0] = x; c[1] = y; c[2] = z;
  return c;
}

// no reverses
int* rot_xyz(int* c)    { return coords( c[0],  c[1],  c[2]); }
int* rot_xzy(int* c)    { return coords( c[0],  c[2],  c[1]); }
int* rot_yxz(int* c)    { return coords( c[1],  c[0],  c[2]); }
int* rot_yzx(int* c)    { return coords( c[1],  c[2],  c[0]); }
int* rot_zxy(int* c)    { return coords( c[2],  c[0],  c[1]); }
int* rot_zyx(int* c)    { return coords( c[2],  c[1],  c[0]); }
// one reverse (x)
int* rot_ixyz(int* c)   { return coords(-c[0],  c[1],  c[2]); }
int* rot_ixzy(int* c)   { return coords(-c[0],  c[2],  c[1]); }
int* rot_yixz(int* c)   { return coords( c[1], -c[0],  c[2]); }
int* rot_yzix(int* c)   { return coords( c[1],  c[2], -c[0]); }
int* rot_zixy(int* c)   { return coords( c[2], -c[0],  c[1]); }
int* rot_zyix(int* c)   { return coords( c[2],  c[1], -c[0]); }
// one reverse (y)
int* rot_xiyz(int* c)   { return coords( c[0], -c[1],  c[2]); }
int* rot_xziy(int* c)   { return coords( c[0],  c[2], -c[1]); }
int* rot_iyxz(int* c)   { return coords(-c[1],  c[0],  c[2]); }
int* rot_iyzx(int* c)   { return coords(-c[1],  c[2],  c[0]); }
int* rot_zxiy(int* c)   { return coords( c[2],  c[0], -c[1]); }
int* rot_ziyx(int* c)   { return coords( c[2], -c[1],  c[0]); }
// one reverse (z)
int* rot_xyiz(int* c)   { return coords( c[0],  c[1], -c[2]); }
int* rot_xizy(int* c)   { return coords( c[0], -c[2],  c[1]); }
int* rot_yxiz(int* c)   { return coords( c[1],  c[0], -c[2]); }
int* rot_yizx(int* c)   { return coords( c[1], -c[2],  c[0]); }
int* rot_izxy(int* c)   { return coords(-c[2],  c[0],  c[1]); }
int* rot_izyx(int* c)   { return coords(-c[2],  c[1],  c[0]); }
// two reverses (x,y)
int* rot_ixiyz(int* c)  { return coords(-c[0], -c[1],  c[2]); }
int* rot_ixziy(int* c)  { return coords(-c[0],  c[2], -c[1]); }
int* rot_iyixz(int* c)  { return coords(-c[1], -c[0],  c[2]); }
int* rot_iyzix(int* c)  { return coords(-c[1],  c[2], -c[0]); }
int* rot_zixiy(int* c)  { return coords( c[2], -c[0], -c[1]); }
int* rot_ziyix(int* c)  { return coords( c[2], -c[1], -c[0]); }
// two reverses (x,z)
int* rot_ixyiz(int* c)  { return coords(-c[0],  c[1], -c[2]); }
int* rot_ixizy(int* c)  { return coords(-c[0], -c[2],  c[1]); }
int* rot_yixiz(int* c)  { return coords( c[1], -c[0], -c[2]); }
int* rot_yizix(int* c)  { return coords( c[1], -c[2], -c[0]); }
int* rot_izixy(int* c)  { return coords(-c[2], -c[0],  c[1]); }
int* rot_izyix(int* c)  { return coords(-c[2],  c[1], -c[0]); }
// two reverses (y,z)
int* rot_xiyiz(int* c)  { return coords( c[0], -c[1], -c[2]); }
int* rot_xiziy(int* c)  { return coords( c[0], -c[2], -c[1]); }
int* rot_iyxiz(int* c)  { return coords(-c[1],  c[0], -c[2]); }
int* rot_iyizx(int* c)  { return coords(-c[1], -c[2],  c[0]); }
int* rot_izxiy(int* c)  { return coords(-c[2],  c[0], -c[1]); }
int* rot_iziyx(int* c)  { return coords(-c[2], -c[1],  c[0]); }
// all reversed
int* rot_ixiyiz(int* c) { return coords(-c[0], -c[1], -c[2]); }
int* rot_ixiziy(int* c) { return coords(-c[0], -c[2], -c[1]); }
int* rot_iyixiz(int* c) { return coords(-c[1], -c[0], -c[2]); }
int* rot_iyizix(int* c) { return coords(-c[1], -c[2], -c[0]); }
int* rot_izixiy(int* c) { return coords(-c[2], -c[0], -c[1]); }
int* rot_iziyix(int* c) { return coords(-c[2], -c[1], -c[0]); }

int* (*rotations[NUM_ROTATIONS])(int*) = {
  rot_xyz, rot_xzy, rot_yxz, rot_yzx, rot_zxy, rot_zyx,
  rot_ixyz, rot_ixzy, rot_yixz, rot_yzix, rot_zixy, rot_zyix,
  rot_xiyz, rot_xziy, rot_iyxz, rot_iyzx, rot_zxiy, rot_ziyx,
  rot_xyiz, rot_xizy, rot_yxiz, rot_yizx, rot_izxy, rot_izyx,
  rot_ixiyz, rot_ixziy, rot_iyixz, rot_iyzix, rot_zixiy, rot_ziyix,
  rot_ixyiz, rot_ixizy, rot_yixiz, rot_yizix, rot_izixy, rot_izyix,
  rot_xiyiz, rot_xiziy, rot_iyxiz, rot_iyizx, rot_izxiy, rot_iziyx,
  rot_ixiyiz, rot_ixiziy, rot_iyixiz, rot_iyizix, rot_izixiy, rot_iziyix
};

void** init_pointer_array(int length) {
  void** arr = malloc(sizeof(void*)*length);
  for (int i=0; i<length; i++) arr[i] = NULL;
  return arr;
}

double** init_distance_matrix(int rows, int cols) {
  double** mtx = (double**)malloc(rows * sizeof(double*));
  for (int i=0; i<rows; i++) {
    mtx[i] = (double*)malloc(cols * sizeof(double));
    memset(mtx[i], 0, cols * sizeof(double));
  }  
  return mtx;
}

Scanner** read_file(FILE* in) {
  Scanner** scanners = (Scanner**) init_pointer_array(MAX_SCANNERS);
  Scanner* scanner = NULL;
  Beacon* beacon = NULL;

  int is = 0;
  int ib;

  char* line;
  ssize_t len = 0;
  char* xyz;

  while (getline(&line, &len, in) >= 0) {
    if (strlen(line) == 1) continue; // skip line break
    else if (line[1] == '-') { // new scanner
      scanner = malloc(sizeof(Scanner));
      scanner->index = is++;
      scanner->beacons = (Beacon**) init_pointer_array(MAX_BEACONS_PER_SCANNER);
      scanner->beaconCount = 0;
      scanner->distances = NULL;
      scanner->origin[0] = UNSET;
      scanner->rotation = NULL;
      scanners[scanner->index] = scanner;
      ib = 0;
    }
    else { // new beacon
      beacon = malloc(sizeof(Beacon));
      xyz = strtok(line, ",");
      beacon->coords[0] = atoi(xyz);
      xyz = strtok(NULL, ",");
      beacon->coords[1] = atoi(xyz);
      xyz = strtok(NULL, ",");
      beacon->coords[2] = atoi(xyz);
      scanner->beacons[scanner->beaconCount++] = beacon;
    }
  }

  return scanners;
}

void print_scanners(Scanner** arr) {
  Scanner* s = NULL;
  Beacon* b = NULL;
  for (int i=0; arr[i] != NULL; i++) {
    s = arr[i];
    printf("--- scanner %d  (%d beacons) ---\n", i, s->beaconCount);
    for (int j=0; s->beacons[j] != NULL; j++) {
      b = s->beacons[j];
      printf("%d,%d,%d\n", b->coords[0], b->coords[1], b->coords[2]);
    }
    printf("--- distances ---\n");
    for (int jx=0; s->beacons[jx] != NULL; jx++) {
      for (int jy=0; s->beacons[jy] != NULL; jy++) {
        if(jx == jy) printf("------- ");
        else printf("%07.2f ", s->distances[jx][jy]);
      }
      printf("\n");
    }
    printf("\n");
  }
}

double distance3d(int* c1, int* c2) {
  return sqrt(
      pow(c2[0]-c1[0], 2)
    + pow(c2[1]-c1[1], 2)
    + pow(c2[2]-c1[2], 2)
  );
}

void calculate_all_distances(Scanner** arr) {
  Scanner* s = NULL;
  double d;
  int* c1 = NULL;
  int* c2 = NULL;
  for (int is=0; arr[is] != NULL; is++) {
    s = arr[is];
    s->distances = init_distance_matrix(s->beaconCount, s->beaconCount);
    for (int bx=0; s->beacons[bx] != NULL; bx++) {
      for (int by=bx+1; s->beacons[by] != NULL; by++) {
        c1 = s->beacons[bx]->coords;
        c2 = s->beacons[by]->coords;
        d = distance3d(c1, c2);
        s->distances[bx][by] = d;
        s->distances[by][bx] = d;
      }
    }
  }
}

void coords_distances_match(int* a1, int* a2, int* b1, int* b2, int* diff) {
  int diff1[3];
  int diff2[3];

  for (int i=0; i<3; i++) {
    diff1[i] = a1[i] - a2[i];
    diff2[i] = b1[i] - b2[i];
  }

  int r = 1;
  for (int i=0; r && i<3; i++) {
    r = r && diff1[i] == diff2[i];
    diff[i] = diff1[i];
  }

  if (!r) diff[0] = UNSET;
}

int check_overlap(Scanner* s1, Scanner* s2) {
  // force s1 to be the already mapped
  Scanner* swp = NULL;
  if (s1->rotation == NULL) { swp = s1; s1 = s2; s2 = swp; }

  // we consider two scanners overlap if at least 12 beacons collide
  // 12 shared beacons mean 66 shared distances (complete 12-node graph)
  int count = 0;
  int* coincidence[2][2];
  for (int b11=0; s1->beacons[b11] != NULL; b11++) {
    for (int b12=b11+1; s1->beacons[b12] != NULL; b12++) {
      for (int b21=0; s2->beacons[b21] != NULL; b21++) {
        for (int b22=b21+1; s2->beacons[b22] != NULL; b22++) {
          if (s1->distances[b11][b12] == s2->distances[b21][b22]) {
            if (count++ == 0) {
              coincidence[0][0] = s1->rotation(s1->beacons[b11]->coords);
              coincidence[0][1] = s1->rotation(s1->beacons[b12]->coords);
              coincidence[1][0] = s2->beacons[b21]->coords;
              coincidence[1][1] = s2->beacons[b22]->coords;
            }
            if (count >= REQUIRED_PAIRS) goto afterloop;
          }
        }
      }
    }
  }
  afterloop:
  if (count < REQUIRED_PAIRS) return 0;
  
  // apply each possible rotation to coincidence[1][x] beacons and try to
  // match them against coincidence[0][x] beacons
  // to match them, compare the difference between beacons of the two scanners
  // and if both differences are equal, we have a match, the current rotation
  // is the right one and we also can determine the offset between scanners
  int* (*rotation)(int*) = NULL;
  int* c11 = coincidence[0][0];
  int* c12 = coincidence[0][1];
  int* c21 = NULL;
  int* c22 = NULL;
  int* cswp = NULL;
  int diff[3];
  for (int ir=0; ir<NUM_ROTATIONS; ir++) {
    rotation = rotations[ir];
    c21 = rotation(coincidence[1][0]);
    c22 = rotation(coincidence[1][1]);

    coords_distances_match(c11, c21, c12, c22, diff);
    cswp = NULL;
    if (diff[0] == UNSET) {
      cswp = c21; c21 = c22; c22 = cswp;
      coords_distances_match(c11, c21, c12, c22, diff);
    }

    if (diff[0] != UNSET) {
      s2->rotation = rotation;
      for (int i=0; i<3; i++)
        s2->origin[i] = diff[i] + s1->origin[i];

      c21 = cswp == NULL ? coincidence[1][0] : coincidence[1][1];
      printf("scanners %d and %d matched\n", s1->index, s2->index);
      printf("rotation applied: %d\n", ir);
      printf("beacon (%d): [%d,%d,%d]\n", s1->index, c11[0], c11[1], c11[2]);
      printf("beacon (%d): [%d,%d,%d]\n", s2->index, c21[0], c21[1], c21[2]);
      printf("scanner %d 0-based origin: [%d,%d,%d]\n", s2->index, s2->origin[0], s2->origin[1], s2->origin[2]);
      printf("\n");

      return 1;
    }
  }

  return 0;
}

int hash2d(int n1, int n2) {
  return ((n1+n2)*(n1+n2+1)+n2) / 2;
}

int hash3d(int n1, int n2, int n3) {
  return (hash2d(n1,n2)+n3)*(hash2d(n1,n2)+n3+1)/2 + n3;
}

struct HeapSet {
  int val;
  struct HeapSet* left;
  struct HeapSet* right;
};
typedef struct HeapSet HeapSet;

HeapSet* heap_add(HeapSet* heap, int val) {
  if (heap == NULL) {
    heap = malloc(sizeof(HeapSet));
    heap->val = val;
    heap->left = NULL;
    heap->right = NULL;
  } else if (heap->val > val) {
    heap->left = heap_add(heap->left, val);
  } else if (heap->val < val) {
    heap->right = heap_add(heap->right, val);
  } else {
    // already added, do nothing
  }
  return heap;
}

int heap_count(HeapSet* heap) {
  if (heap == NULL) return 0;
  return 1 + heap_count(heap->left) + heap_count(heap->right);
}

int get_unique_beacons(Scanner** scanners) {
  // Scanner 0 used as reference (0,0,0 and no rotation)
  scanners[0]->origin[0] = 0;
  scanners[0]->origin[1] = 0;
  scanners[0]->origin[2] = 0;
  scanners[0]->rotation = rot_xyz;

  while(1) {
    for (int si=0; scanners[si] != NULL; si++) {
      for (int sj=0; scanners[sj] != NULL; sj++) {
        if (si != sj && (scanners[si]->rotation == NULL ^ scanners[sj]->rotation == NULL)) {
          // two different scanners, one already mapped the other one not, let's
          // check if they overlap
          if (check_overlap(scanners[si], scanners[sj])) continue;
        }
      }
    }
    
    // all paired or no more overlaps detected, stop checking
    break;
  }

  for (int si=0; scanners[si] != NULL; si++) {
    if (scanners[si]->rotation == NULL) {
      printf("Scanner %d could not be mapped.\n", si);
      exit(1);
    }
  }

  // rotate, cross, hash a set-save all beacons
  HeapSet* h = NULL;
  Scanner* s = NULL;
  int* c = NULL;
  int hash;
  for (int si=0; scanners[si] != NULL; si++) {
    s = scanners[si];
    for (int bi=0; s->beacons[bi] != NULL; bi++) {
      c = s->rotation(s->beacons[bi]->coords);
      c[0] -= s->origin[0];
      c[1] -= s->origin[1];
      c[2] -= s->origin[2];
      h = heap_add(h, hash3d(c[0], c[1], c[2]));
    }
  }

  return heap_count(h);
}

int main(int argc, char** args) {
  FILE *fp = NULL;
  Scanner** scanners = NULL; 
  int unique_beacons;

  if (argc != 2) {
    printf("Invalid arguments: <input_file>\n");
    exit(1);
  }

  args[1] = "19-1/sample.in";
  if ((fp = fopen(args[1], "r")) == NULL) {
    printf("Cannot open file: %s\n", args[1]);
    exit(1);
  }

  scanners = read_file(fp);
  calculate_all_distances(scanners);
  //print_scanners(scanners);

  printf("Answer: %d\n", get_unique_beacons(scanners));

  fclose(fp);
  exit(0);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAPSIZE 71

struct pos {
  int x;
  int y;
};

enum dir { UP, DOWN, LEFT, RIGHT };

char mem[MAPSIZE][MAPSIZE];

void shortestPath(int **costs, struct pos **prevs) {
  struct pos stack[MAPSIZE * MAPSIZE];
  int sp = 0;
  stack[sp++] = (struct pos){0, 0};
  costs[0][0] = 0;

  while (sp) {
    struct pos current = stack[--sp];
    int curCost = costs[current.y][current.x];
    if (curCost < 0) {
      return;
    }
    for (int d = 0; d < 4; d++) {
      if ((d == UP && current.y == 0) ||
          (d == DOWN && current.y == MAPSIZE - 1) ||
          (d == LEFT && current.x == 0) ||
          (d == RIGHT && current.x == MAPSIZE - 1)) {
        continue;
      }
      struct pos next = current;
      switch (d) {
      case UP:
        next.y--;
        break;
      case DOWN:
        next.y++;
        break;
      case LEFT:
        next.x--;
        break;
      case RIGHT:
        next.x++;
        break;
      }
      if (mem[next.y][next.x] == '#') {
        continue;
      }
      //   printf("c %d %d n %d %d\n", current.x, current.y, next.x, next.y);
      int nextCost = curCost + 1;
      if (costs[next.y][next.x] == -1 || nextCost < costs[next.y][next.x]) {
        costs[next.y][next.x] = nextCost;
        prevs[next.y][next.x] = current;
        stack[sp++] = next;
      }
    }
  }
}

void loadMem(char *path, int fal, struct pos *last) {
  FILE *f = fopen(path, "r");
  int x, y;
  while (!feof(f) && fal) {
    fscanf(f, "%d,%d\n", &x, &y);
    mem[y][x] = '#';
    fal--;
  }
  last->x = x;
  last->y = y;
  fclose(f);
}

void printMem() {
  for (int i = 0; i < MAPSIZE; i++) {
    for (int j = 0; j < MAPSIZE; j++) {
      printf("%c", mem[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

void drawPath(struct pos **prevs, struct pos to) {
  while (to.x != 0 || to.y != 0) {
    mem[to.y][to.x] = 'O';
    to = prevs[to.y][to.x];
  }
}

int main() {
  int **costs = malloc(MAPSIZE * sizeof(int *));
  struct pos **prevs = malloc(MAPSIZE * sizeof(struct pos *));
  for (int i = 0; i < MAPSIZE; i++) {
    costs[i] = malloc(MAPSIZE * sizeof(int));
    prevs[i] = malloc(MAPSIZE * sizeof(struct pos));
    for (int j = 0; j < MAPSIZE; j++) {
      costs[i][j] = -1;
      mem[i][j] = '.';
    }
  }
  struct pos last;
  loadMem("data/18.txt", 1024, &last);

  shortestPath(costs, prevs);
  drawPath(prevs, (struct pos){MAPSIZE - 1, MAPSIZE - 1});
  printMem();

  printf("Min cost %d\n", costs[MAPSIZE - 1][MAPSIZE - 1]);
  int minK = 1024, maxK = 100000;
  int k = minK + (maxK - minK) / 2;
  while (1) {
    for (int i = 0; i < MAPSIZE; i++) {
      for (int j = 0; j < MAPSIZE; j++) {
        prevs[i][j] = (struct pos){0, 0};
        costs[i][j] = -1;
        mem[i][j] = '.';
      }
    }
    loadMem("data/18.txt", k, &last);
    shortestPath(costs, prevs);
    if (costs[MAPSIZE - 1][MAPSIZE - 1] == -1) {
      printf("Closed at %d: %d,%d\n", k, last.x, last.y);
      if (minK == maxK - 1) {
        break;
      }
      maxK = k;
    } else {
      if (minK == maxK - 1) {
        break;
      }
      minK = k;
    }
    k = minK + (maxK - minK) / 2;
  }

  for (int i = 0; i < MAPSIZE; i++) {
    free(costs[i]);
    free(prevs[i]);
  }
  free(costs);
  free(prevs);
  return 0;
}
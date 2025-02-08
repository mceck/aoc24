#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct freqMap {
  int rowLen;
  int colLen;
  char **map;
};

struct pos {
  int x;
  int y;
};

struct posList {
  struct pos *pos;
  struct posList *next;
};

struct freqMap *createFreqMap(char *path) {
  FILE *file = fopen(path, "r");
  struct freqMap *map = malloc(sizeof(struct freqMap));
  map->rowLen = 0;
  map->colLen = 0;
  map->map = NULL;
  while (feof(file) == 0) {
    char buffer[BUFSIZ];
    fgets(buffer, BUFSIZ, file);
    if (!map->rowLen) {
      map->rowLen = strlen(buffer) - 1;
    }
    map->map = realloc(map->map, sizeof(char *) * (map->colLen + 1));
    map->map[map->colLen] = malloc(map->rowLen);
    strncpy(map->map[map->colLen], buffer, map->rowLen);
    map->colLen++;
  }

  fclose(file);
  return map;
}

void addPos(struct posList **h, char k, struct pos *p) {
  struct posList *l = malloc(sizeof(struct posList));
  l->pos = p;
  l->next = h[k];
  h[k] = l;
}

void freePosMap(struct posList **posMap) {
  for (int i = 0; i < 256; i++) {
    struct posList *l = posMap[i];
    while (l) {
      struct posList *tmp = l;
      l = l->next;
      free(tmp->pos);
      free(tmp);
    }
  }
}

int applyResonance(struct freqMap *map, int breakResonance) {
  int resonanceCount = 0;
  struct posList *posMap[256] = {0};
  char **resonances = malloc(sizeof(char *) * map->colLen);
  // Create a map letter -> positions
  for (int i = 0; i < map->colLen; i++) {
    resonances[i] = malloc(sizeof(char) * map->rowLen);
    for (int j = 0; j < map->rowLen; j++) {
      resonances[i][j] = '.';
      char c = map->map[i][j];
      if (c == '#' || c == '.') {
        continue;
      }
      struct pos *p = malloc(sizeof(struct pos));
      p->x = j;
      p->y = i;
      addPos(posMap, c, p);
    }
  }

  // for each letter, check if there are two positions in resonance
  for (int i = 0; i < 256; i++) {
    struct posList *l = posMap[i];
    if (!l) {
      continue;
    }
    struct posList *c = posMap[i];
    int count = 0;
    while (c) {
      count++;
      c = c->next;
    }
    if (count < 2) {
      continue;
    }
    if (breakResonance == 0)
      resonanceCount += count;

    while (l) {
      struct posList *l2 = l->next;

      while (l2) {
        struct pos *p1 = l->pos;
        struct pos *p2 = l2->pos;
        int xDist = p1->x - p2->x;
        int yDist = p1->y - p2->y;

        int newX1 = p1->x + xDist;
        int newY1 = p1->y + yDist;

        while (newX1 >= 0 && newX1 < map->rowLen && newY1 >= 0 &&
               newY1 < map->colLen) {
          if (resonances[newY1][newX1] != '#' &&
              (breakResonance || map->map[newY1][newX1] == '.')) {
            resonances[newY1][newX1] = '#';
            resonanceCount++;
          }
          if (breakResonance)
            break;
          newX1 += xDist;
          newY1 += yDist;
        }

        int newX2 = p2->x - xDist;
        int newY2 = p2->y - yDist;
        while (newX2 >= 0 && newX2 < map->rowLen && newY2 >= 0 &&
               newY2 < map->colLen) {
          if (resonances[newY2][newX2] != '#' &&
              (breakResonance || map->map[newY2][newX2] == '.')) {
            resonances[newY2][newX2] = '#';
            resonanceCount++;
          }
          if (breakResonance)
            break;
          newX2 -= xDist;
          newY2 -= yDist;
        }

        l2 = l2->next;
      }
      l = l->next;
    }
  }

  for (int i = 0; i < map->colLen; i++) {
    free(resonances[i]);
  }

  free(resonances);
  freePosMap(posMap);
  return resonanceCount;
}

void freeFreqMap(struct freqMap *map) {
  for (int i = 0; i < map->colLen; i++) {
    free(map->map[i]);
  }
  free(map->map);
  free(map);
}

int main() {
  struct freqMap *map = createFreqMap("data/8.txt");
  int r = applyResonance(map, 1);
  printf("Resonance count: %d\n", r);
  r = applyResonance(map, 0);
  printf("Resonance count2: %d\n", r);

  freeFreqMap(map);
  return 0;
}
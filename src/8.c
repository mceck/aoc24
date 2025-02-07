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

// FIXME: This function is not working properly
int applyResonance(struct freqMap *map) {
  int resonanceCount = 0;
  struct posList *posMap[256] = {0};
  // Create a map letter -> positions
  for (int i = 0; i < map->colLen; i++) {
    for (int j = 0; j < map->rowLen; j++) {
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

    while (l) {
      printf("%c: %d %d\n", i, l->pos->x, l->pos->y);
      struct posList *l2 = l->next;
      while (l2) {
        struct pos *p1 = l->pos;
        struct pos *p2 = l2->pos;
        int xDist = p1->x - p2->x;
        int yDist = p1->y - p2->y;

        int newX1 = p1->x + xDist;
        int newY1 = p1->y + yDist;
        int newX2 = p2->x - xDist;
        int newY2 = p2->y - yDist;

        if (newX1 >= 0 && newX1 < map->rowLen && newY1 >= 0 &&
            newY1 < map->colLen) {
          if (map->map[newY1][newX1] == '.' || map->map[newY1][newX1] == '#') {
            printf("aResonance detected between %d %d and %d %d -> %d %d\n",
                   p1->x, p1->y, p2->x, p2->y, newX1, newY1);
            map->map[newY1][newX1] = '#';
            resonanceCount++;
          }
        }

        if (newX2 >= 0 && newX2 < map->rowLen && newY2 >= 0 &&
            newY2 < map->colLen) {
          if (map->map[newY2][newX2] == '.' || map->map[newY2][newX2] == '#') {
            printf("bResonance detected between %d %d and %d %d -> %d %d\n",
                   p1->x, p1->y, p2->x, p2->y, newX2, newY2);
            map->map[newY2][newX2] = '#';
            resonanceCount++;
          }
        }

        l2 = l2->next;
      }
      l = l->next;
    }
  }
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
  for (int i = 0; i < map->colLen; i++) {
    for (int j = 0; j < map->rowLen; j++) {
      printf("%c", map->map[i][j]);
    }
    printf("\n");
  }
  printf("\n");
  printf("\n");
  int r = applyResonance(map);
  printf("\n");
  printf("\n");
  for (int i = 0; i < map->colLen; i++) {
    for (int j = 0; j < map->rowLen; j++) {
      printf("%c", map->map[i][j]);
    }
    printf("\n");
  }
  printf("\n");
  printf("\n");
  printf("Resonance count: %d\n", r);

  freeFreqMap(map);
  return 0;
}
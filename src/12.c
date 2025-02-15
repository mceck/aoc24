#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct fmap {
  int rowLen;
  int colLen;
  char **map;
};

struct fmap *createFmap(char *path) {
  FILE *file = fopen(path, "r");
  struct fmap *map = malloc(sizeof(struct fmap));
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

void freeMap(struct fmap *map) {
  for (int i = 0; i < map->colLen; i++) {
    free(map->map[i]);
  }
  free(map->map);
  free(map);
}

void printMap(struct fmap *map) {
  for (int i = 0; i < map->colLen; i++) {
    printf("%s\n", map->map[i]);
  }
  printf("\n");
}

int getUnitCost(int i, int j, struct fmap *map, int *visited, int *fences) {
  if (visited[i * map->rowLen + j] != 0 || i < 0 || j < 0 || i >= map->colLen ||
      j >= map->rowLen) {
    return 0;
  }
  visited[i * map->rowLen + j] = 1;

  int left = j - 1;
  int right = j + 1;
  int up = i - 1;
  int down = i + 1;

  // count fences
  if (left < 0 || map->map[i][left] != map->map[i][j]) {
    (*fences)++;
  }
  if (right >= map->rowLen || map->map[i][right] != map->map[i][j]) {
    (*fences)++;
  }
  if (up < 0 || map->map[up][j] != map->map[i][j]) {
    (*fences)++;
  }
  if (down >= map->colLen || map->map[down][j] != map->map[i][j]) {
    (*fences)++;
  }

  // size
  int res = 0;
  if (left >= 0 && map->map[i][left] == map->map[i][j]) {
    res += getUnitCost(i, left, map, visited, fences);
  }
  if (right < map->rowLen && map->map[i][right] == map->map[i][j]) {
    res += getUnitCost(i, right, map, visited, fences);
  }
  if (up >= 0 && map->map[up][j] == map->map[i][j]) {
    res += getUnitCost(up, j, map, visited, fences);
  }
  if (down < map->colLen && map->map[down][j] == map->map[i][j]) {
    res += getUnitCost(down, j, map, visited, fences);
  }

  return res + 1;
}

int getSideCost(int i, int j, struct fmap *map, int *visited, int *fences) {
  if (visited[i * map->rowLen + j] != 0 || i < 0 || j < 0 || i >= map->colLen ||
      j >= map->rowLen) {
    return 0;
  }
  visited[i * map->rowLen + j] = 1;

  int left = j - 1;
  int right = j + 1;
  int up = i - 1;
  int down = i + 1;

  // concave corners
  if ((left < 0 || map->map[i][left] != map->map[i][j]) &&
      (up < 0 || map->map[up][j] != map->map[i][j])) {
    (*fences)++;
  }
  if ((left < 0 || map->map[i][left] != map->map[i][j]) &&
      (down >= map->colLen || map->map[down][j] != map->map[i][j])) {
    (*fences)++;
  }
  if ((right >= map->rowLen || map->map[i][right] != map->map[i][j]) &&
      (up < 0 || map->map[up][j] != map->map[i][j])) {
    (*fences)++;
  }
  if ((right >= map->rowLen || map->map[i][right] != map->map[i][j]) &&
      (down >= map->colLen || map->map[down][j] != map->map[i][j])) {
    (*fences)++;
  }

  // convex corners
  if ((left >= 0 && map->map[i][left] == map->map[i][j]) &&
      (up >= 0 && map->map[up][j] == map->map[i][j]) &&
      map->map[up][left] != map->map[i][j]) {
    (*fences)++;
  }
  if ((left >= 0 && map->map[i][left] == map->map[i][j]) &&
      (down < map->colLen && map->map[down][j] == map->map[i][j]) &&
      map->map[down][left] != map->map[i][j]) {
    (*fences)++;
  }
  if ((right < map->rowLen && map->map[i][right] == map->map[i][j]) &&
      (up >= 0 && map->map[up][j] == map->map[i][j]) &&
      map->map[up][right] != map->map[i][j]) {
    (*fences)++;
  }
  if ((right < map->rowLen && map->map[i][right] == map->map[i][j]) &&
      (down < map->colLen && map->map[down][j] == map->map[i][j]) &&
      map->map[down][right] != map->map[i][j]) {
    (*fences)++;
  }

  // size
  int res = 0;
  if (left >= 0 && map->map[i][left] == map->map[i][j]) {
    res += getSideCost(i, left, map, visited, fences);
  }
  if (right < map->rowLen && map->map[i][right] == map->map[i][j]) {
    res += getSideCost(i, right, map, visited, fences);
  }
  if (up >= 0 && map->map[up][j] == map->map[i][j]) {
    res += getSideCost(up, j, map, visited, fences);
  }
  if (down < map->colLen && map->map[down][j] == map->map[i][j]) {
    res += getSideCost(down, j, map, visited, fences);
  }

  return res + 1;
}

int getFenceCost(struct fmap *map, int onlySides) {
  int totalFences = 0;

  int *visited = calloc(map->rowLen * map->colLen, sizeof(int));
  for (int i = 0; i < map->colLen; i++) {
    for (int j = 0; j < map->rowLen; j++) {
      int fences = 0;
      int w = onlySides == 0 ? getUnitCost(i, j, map, visited, &fences)
                             : getSideCost(i, j, map, visited, &fences);
      totalFences += (fences * w);
    }
  }
  free(visited);

  return totalFences;
}

int main() {
  struct fmap *map = createFmap("data/12.txt");

  int cost = getFenceCost(map, 0);
  printf("Cost: %d\n", cost);
  cost = getFenceCost(map, 1);
  printf("Cost2: %d\n", cost);

  freeMap(map);
  return 0;
}
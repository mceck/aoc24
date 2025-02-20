#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct pos {
  int x;
  int y;
};

struct map {
  int width;
  int height;
  char **map;
  char *moves;
  struct pos pos;
};

struct map *loadMap(char *path) {
  FILE *file = fopen(path, "r");
  struct map *map = malloc(sizeof(struct map));
  map->width = 0;
  map->height = 0;
  map->map = NULL;
  char buffer[BUFSIZ];
  int playerFound = 0;

  while (feof(file) == 0) {
    fgets(buffer, BUFSIZ, file);
    int len = strlen(buffer);
    if (!map->width) {
      map->width = len - 1;
    }
    if (len == 1) {
      break;
    }
    map->map = realloc(map->map, sizeof(char *) * (map->height + 1));
    map->map[map->height] = malloc(map->width);
    strncpy(map->map[map->height], buffer, map->width);
    if (!playerFound) {
      for (int i = 0; i < map->width; i++) {
        if (map->map[map->height][i] == '@') {
          map->pos.x = i;
          map->pos.y = map->height;
          playerFound = 1;
        }
      }
    }
    map->height++;
  }
  int movesCount = 0;
  while (feof(file) == 0) {
    fgets(buffer, BUFSIZ, file);
    int len = strlen(buffer);
    if (buffer[len - 1] == '\n')
      buffer[--len] = '\0';
    map->moves = realloc(map->moves, movesCount + len + 1);
    strcpy(map->moves + movesCount, buffer);
    movesCount += len;
  }

  fclose(file);
  return map;
}

struct map *loadMap2(char *path) {
  FILE *file = fopen(path, "r");
  struct map *map = malloc(sizeof(struct map));
  map->width = 0;
  map->height = 0;
  map->map = NULL;
  char buffer[BUFSIZ];
  int playerFound = 0;

  while (feof(file) == 0) {
    fgets(buffer, BUFSIZ, file);
    int len = strlen(buffer);
    if (!map->width) {
      map->width = (len - 1) * 2;
    }
    if (len == 1) {
      break;
    }
    map->map = realloc(map->map, sizeof(char *) * (map->height + 1));
    map->map[map->height] = malloc(map->width);
    char lastBrick = ']';
    for (int i = 0; i < map->width; i++) {
      map->map[map->height][i] = buffer[i / 2];
      if (buffer[i / 2] == '@') {
        if (playerFound) {
          map->map[map->height][i] = '.';
        } else {
          map->pos.x = i;
          map->pos.y = map->height;
          playerFound = 1;
        }
      } else if (buffer[i / 2] == 'O') {
        if (lastBrick == '[') {
          map->map[map->height][i] = ']';
        } else {
          map->map[map->height][i] = '[';
        }
        lastBrick = map->map[map->height][i];
      }
    }

    map->height++;
  }
  int movesCount = 0;
  while (feof(file) == 0) {
    fgets(buffer, BUFSIZ, file);
    int len = strlen(buffer);
    if (buffer[len - 1] == '\n')
      buffer[--len] = '\0';
    map->moves = realloc(map->moves, movesCount + len + 1);
    strcpy(map->moves + movesCount, buffer);
    movesCount += len;
  }

  fclose(file);
  return map;
}

void freeMap(struct map *map) {
  for (int i = 0; i < map->height; i++) {
    free(map->map[i]);
  }
  free(map->map);
  free(map);
}

int getNextX(int x, char dir) {
  if (dir == '<')
    return x - 1;
  if (dir == '>')
    return x + 1;
  return x;
}

int getNextY(int y, char dir) {
  if (dir == '^')
    return y - 1;
  if (dir == 'v')
    return y + 1;
  return y;
}

int canShift(struct map *map, int x, int y, char dir) {
  if (map->map[y][x] == '#') {
    return 0;
  }
  if (map->map[y][x] == '.') {
    return 1;
  }

  int nextX = getNextX(x, dir);
  int nextY = getNextY(y, dir);
  int r = canShift(map, nextX, nextY, dir);
  if (r && (dir == '^' || dir == 'v') &&
      (map->map[nextY][nextX] == '[' || map->map[nextY][nextX] == ']')) {
    int nextX2 =
        getNextX(map->map[nextY][nextX] == '[' ? nextX + 1 : nextX - 1, dir);
    r = canShift(map, nextX2, nextY, dir);
  }
  return r;
}

void printMap(struct map *map) {
  for (int i = 0; i < map->height; i++) {
    printf("%s\n", map->map[i]);
  }
  printf("\n");
}

void getShifts(struct map *map, int x, int y, int *toBeMoved, char move) {
  if (toBeMoved[y * map->width + x]) {
    return;
  }
  toBeMoved[y * map->width + x] = 1;
  while (map->map[y][x] != '.') {
    // handle large bricks
    if ((move == '^' || move == 'v') &&
        (map->map[y][x] == '[' || map->map[y][x] == ']')) {
      int nx = map->map[y][x] == '[' ? x + 1 : x - 1;
      getShifts(map, nx, y, toBeMoved, move);
    }

    x = getNextX(x, move);
    y = getNextY(y, move);
    if (map->map[y][x] != '.') {
      toBeMoved[y * map->width + x] = 1;
    }
  }
}

void moveRobot(struct map *map) {
  int *toBeMoved = calloc(map->height * map->width, sizeof(int));
  for (int i = 0; i < strlen(map->moves); i++) {
    char move = map->moves[i];
    if (!canShift(map, map->pos.x, map->pos.y, move)) {
      continue;
    }
    getShifts(map, map->pos.x, map->pos.y, toBeMoved, move);
    for (int kt = 0; kt < map->height; kt++) {
      int k = kt;
      if (move == 'v') {
        k = map->height - kt - 1;
      }
      for (int jt = 0; jt < map->width; jt++) {
        int j = jt;
        if (move == '>') {
          j = map->width - jt - 1;
        }
        if (toBeMoved[k * map->width + j]) {
          int nextX = getNextX(j, move);
          int nextY = getNextY(k, move);
          char brick = map->map[k][j];
          map->map[k][j] = map->map[nextY][nextX];
          map->map[nextY][nextX] = brick;

          if (brick == '@') {
            map->pos.x = nextX;
            map->pos.y = nextY;
          }
        }
      }
    }
    memset(toBeMoved, 0, map->height * map->width * sizeof(int));
  }

  free(toBeMoved);
}

int sumBoxGps(struct map *map, char brick) {
  int sum = 0;
  for (int i = 0; i < map->height; i++) {
    for (int j = 0; j < map->width; j++) {
      if (map->map[i][j] == brick) {
        sum += (100 * i + j);
      }
    }
  }
  return sum;
}

int main() {
  struct map *map = loadMap("data/15.txt");
  moveRobot(map);
  int tot = sumBoxGps(map, 'O');
  printf("Total: %d\n", tot);
  if (tot != 1446158) {
    printf("Failed\n");
    return 1;
  }
  freeMap(map);

  map = loadMap2("data/15.txt");
  moveRobot(map);
  tot = sumBoxGps(map, '[');
  printf("Total2: %d\n", tot);
  freeMap(map);
  return 0;
}
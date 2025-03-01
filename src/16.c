#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TURN_COST 1000
#define STRAIGHT_COST 1

enum direction { UP, DOWN, LEFT, RIGHT };

struct pos {
  int x;
  int y;
};

struct posDir {
  struct pos pos;
  enum direction direction;
};

struct posList {
  struct pos pos;
  enum direction direction;
  struct posList *next;
};

struct map {
  int width;
  int height;
  char **map;
  struct pos start;
  struct pos end;
};

struct map *loadMap(char *path) {
  FILE *file = fopen(path, "r");
  struct map *map = malloc(sizeof(struct map));
  map->width = 0;
  map->height = 0;
  map->map = NULL;
  char buffer[BUFSIZ];

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
    for (int i = 0; i < map->width; i++) {
      map->map[map->height][i] = buffer[i];
      if (buffer[i] == 'S') {
        map->start.x = i;
        map->start.y = map->height;
      } else if (buffer[i] == 'E') {
        map->end.x = i;
        map->end.y = map->height;
      }
    }
    map->height++;
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

int getMoveCost(struct map *map, struct pos current,
                enum direction currentDirection, enum direction newDirection) {
  switch (newDirection) {
  case UP:
    if (currentDirection == newDirection &&
        (current.y == 0 || map->map[current.y - 1][current.x] == '#')) {
      return -1;
    }
    if (currentDirection == UP) {
      return STRAIGHT_COST;
    } else if (currentDirection == DOWN) {
      return -1;
    }
    return TURN_COST;
  case DOWN:
    if (currentDirection == newDirection &&
        (current.y == map->height - 1 ||
         map->map[current.y + 1][current.x] == '#')) {
      return -1;
    }
    if (currentDirection == DOWN) {
      return STRAIGHT_COST;
    } else if (currentDirection == UP) {
      return -1;
    }
    return TURN_COST;

  case LEFT:
    if (currentDirection == newDirection &&
        (current.x == 0 || map->map[current.y][current.x - 1] == '#')) {
      return -1;
    }
    if (currentDirection == LEFT) {
      return STRAIGHT_COST;
    } else if (currentDirection == RIGHT) {
      return -1;
    }
    return TURN_COST;
  case RIGHT:
    if (currentDirection == newDirection &&
        (current.x == map->width - 1 ||
         map->map[current.y][current.x + 1] == '#')) {
      return -1;
    }
    if (currentDirection == RIGHT) {
      return STRAIGHT_COST;
    } else if (currentDirection == LEFT) {
      return -1;
    }
    return TURN_COST;
  }
  return TURN_COST;
}

void clearList(struct posList *l) {
  if (!l) {
    return;
  }
  if (l->next) {
    clearList(l->next);
  }
  free(l);
}

int getNodeIndex(struct map *map, struct pos p, enum direction d) {
  return p.y * map->width + p.x + map->width * map->height * d;
}

void getCosts(struct map *map, int *costs, struct posList **prevs) {
  enum direction currentDirection = RIGHT;
  int currentId = getNodeIndex(map, map->start, currentDirection);
  costs[currentId] = 0;
  struct posDir queue[map->height * map->width * 4];
  int qp = 0;
  queue[qp++] =
      (struct posDir){.pos = map->start, .direction = currentDirection};
  while (qp) {
    struct posDir pd = queue[--qp];
    currentId = getNodeIndex(map, pd.pos, pd.direction);
    if (costs[currentId] == -1) {
      break;
    }

    for (int i = 0; i < 4; i++) {
      int moveCost = getMoveCost(map, pd.pos, pd.direction, i);
      if (moveCost == -1) {
        continue;
      }
      int newCost = costs[currentId] + moveCost;
      struct pos newPos = pd.pos;
      if (moveCost == STRAIGHT_COST) {
        switch (i) {
        case UP:
          newPos.y--;
          break;
        case DOWN:
          newPos.y++;
          break;
        case LEFT:
          newPos.x--;
          break;
        case RIGHT:
          newPos.x++;
          break;
        }
      }
      int newId = getNodeIndex(map, newPos, i);
      if (costs[newId] == -1 || newCost <= costs[newId]) {
        if (costs[newId] == -1 || newCost < costs[newId]) {
          clearList(prevs[newId]);
          prevs[newId] = NULL;
          queue[qp++] = (struct posDir){.pos = newPos, .direction = i};
        }
        costs[newId] = newCost;
        struct posList *l = malloc(sizeof(struct posList));
        l->pos = pd.pos;
        l->direction = pd.direction;
        l->next = prevs[newId];
        prevs[newId] = l;
      }
    }
  }
}

int visited(struct map *map, struct posList **prevs, struct pos current,
            enum direction dir, int *alreadyVisited) {
  int id = getNodeIndex(map, current, dir);
  if (current.x == map->start.x && current.y == map->start.y) {
    return 1;
  }
  struct posList *l = prevs[id];
  int r = 0;
  while (l) {
    int nid = getNodeIndex(map, l->pos, l->direction);
    if (alreadyVisited[nid]) {
      return 1;
    }
    int t = visited(map, prevs, l->pos, l->direction, alreadyVisited);
    if (t)
      alreadyVisited[nid] = 1;
    r = r || t;
    l = l->next;
  }
  return r;
}

int countAndPrint(struct map *map, int *alreadyVisited) {
  int visited = 0;
  for (int i = 0; i < map->height; i++) {
    for (int j = 0; j < map->width; j++) {
      if (alreadyVisited[i * map->width + j] ||
          alreadyVisited[i * map->width + j + map->height * map->width] ||
          alreadyVisited[i * map->width + j + 2 * map->height * map->width] ||
          alreadyVisited[i * map->width + j + 3 * map->height * map->width]) {
        visited++;
        printf("O");
      } else {
        printf("%c", map->map[i][j]);
      }
    }
    printf("\n");
  }
  printf("\n");
  return visited;
}

int findBestPath(struct map *map) {
  int *costs = malloc(map->height * map->width * 4 * sizeof(int));
  struct posList **prevs =
      malloc(map->height * map->width * 4 * sizeof(struct posList *));
  for (int i = 0; i < map->height * map->width * 4; i++) {
    costs[i] = -1;
    prevs[i] = NULL;
  }

  struct pos current = map->start;
  enum direction currentDirection = RIGHT;

  getCosts(map, costs, prevs);
  int result = -1;
  enum direction lastDir;
  for (int i = 0; i < 4; i++) {
    int id = getNodeIndex(map, map->end, i);
    if (costs[id] != -1 && (result == -1 || costs[id] < result)) {
      result = costs[id];
      lastDir = i;
    }
  }

  int *alreadyVisited = calloc(map->height * map->width * 4, sizeof(int));
  alreadyVisited[getNodeIndex(map, map->end, lastDir)] = 1;
  visited(map, prevs, map->end, lastDir, alreadyVisited);

  int v = countAndPrint(map, alreadyVisited);
  printf("Visited: %d\n", v);

  free(alreadyVisited);
  free(costs);
  for (int i = 0; i < map->height * map->width * 4; i++) {
    clearList(prevs[i]);
  }
  free(prevs);
  return result;
}

int main() {
  struct map *map = loadMap("data/16.txt");
  int bestPath = findBestPath(map);
  printf("Best path: %d\n", bestPath);
  freeMap(map);
}
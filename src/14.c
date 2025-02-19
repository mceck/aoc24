#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct vec {
  int x;
  int y;
};

struct robot {
  struct vec pos;
  struct vec vel;
};

struct map {
  int width;
  int height;
  int robotCount;
  struct robot *robots;
};

struct map *createMap(char *path, int width, int height) {
  FILE *file = fopen(path, "r");
  struct map *map = malloc(sizeof(struct map));
  map->width = width;
  map->height = height;
  map->robots = NULL;
  while (feof(file) == 0) {
    struct robot robot;
    fscanf(file, "p=%d,%d v=%d,%d\n", &robot.pos.x, &robot.pos.y, &robot.vel.x,
           &robot.vel.y);
    map->robots =
        realloc(map->robots, sizeof(struct robot) * (map->robotCount + 1));
    map->robots[map->robotCount] = robot;
    map->robotCount++;
  }

  fclose(file);
  return map;
}

void moveRobots(struct map *map) {
  for (int i = 0; i < map->robotCount; i++) {
    if (map->robots[i].pos.x + map->robots[i].vel.x < 0) {
      int delta = abs(map->robots[i].pos.x + map->robots[i].vel.x);
      map->robots[i].pos.x = map->width - delta;
    } else {
      map->robots[i].pos.x =
          (map->robots[i].pos.x + map->robots[i].vel.x) % map->width;
    }
    if (map->robots[i].pos.y + map->robots[i].vel.y < 0) {
      int delta = abs(map->robots[i].pos.y + map->robots[i].vel.y);
      map->robots[i].pos.y = map->height - delta;
    } else {
      map->robots[i].pos.y =
          (map->robots[i].pos.y + map->robots[i].vel.y) % map->height;
    }
  }
}

void destroyMap(struct map *map) {
  free(map->robots);
  free(map);
}

int getSafetyFactor(struct map *map) {
  int tl = 0, bl = 0, tr = 0, br = 0;
  for (int i = 0; i < map->robotCount; i++) {
    if (map->robots[i].pos.x < map->width / 2) {
      if (map->robots[i].pos.y < map->height / 2) {
        tl++;
      } else if (map->robots[i].pos.y > map->height / 2) {
        bl++;
      }
    } else if (map->robots[i].pos.x > map->width / 2) {
      if (map->robots[i].pos.y < map->height / 2) {
        tr++;
      } else if (map->robots[i].pos.y > map->height / 2) {
        br++;
      }
    }
  }
  printf("tl: %d, bl: %d, tr: %d, br: %d\n", tl, bl, tr, br);
  return tl * bl * tr * br;
}

int noCollision(struct map *map) {
  for (int i = 0; i < map->robotCount; i++) {
    for (int j = i + 1; j < map->robotCount; j++) {
      if (map->robots[i].pos.x == map->robots[j].pos.x &&
          map->robots[i].pos.y == map->robots[j].pos.y && i != j) {
        return 1;
      }
    }
  }
  return 0;
}

int main() {
  struct map *map = createMap("data/14.txt", 101, 103);
  int steps = 0;
  while (noCollision(map) || !steps) {
    moveRobots(map);
    steps++;
    if (steps == 100) {
      int safety = getSafetyFactor(map);
      printf("Safety at 100s: %d\n", safety);
    }
  }
  printf("Steps: %d\n", steps);
  destroyMap(map);
  return 0;
}
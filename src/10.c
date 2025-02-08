#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct tmap {
  int rowLen;
  int colLen;
  char **map;
};

struct tmap *createTmap(char *path) {
  FILE *file = fopen(path, "r");
  struct tmap *map = malloc(sizeof(struct tmap));
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

int dfs(struct tmap *map, int r, int c, int current_height, int *visited) {
  if (r < 0 || r >= map->rowLen || c < 0 || c >= map->colLen) {
    return 0;
  }
  int val = map->map[r][c] - '0';
  if (val != current_height) {
    return 0;
  }
  if (current_height == 9) {
    if (visited && visited[r * map->rowLen + c]) {
      return 0;
    }
    if (visited)
      visited[r * map->rowLen + c] = 1;
    return 1;
  }

  int count = 0;
  int dr[] = {-1, 1, 0, 0};
  int dc[] = {0, 0, -1, 1};

  for (int i = 0; i < 4; i++) {
    count += dfs(map, r + dr[i], c + dc[i], current_height + 1, visited);
  }
  return count;
}

void freeTmap(struct tmap *map) {
  for (int i = 0; i < map->colLen; i++) {
    free(map->map[i]);
  }
  free(map->map);
  free(map);
}

int main() {
  struct tmap *map = createTmap("data/10.txt");
  int *visited = calloc(map->rowLen * map->colLen, sizeof(int));
  int total_score = 0;
  int total_score2 = 0;
  for (int i = 0; i < map->colLen; i++) {
    for (int j = 0; j < map->rowLen; j++) {
      if (map->map[i][j] == '0') {
        memset(visited, 0, map->rowLen * map->colLen * sizeof(int));
        total_score += dfs(map, i, j, 0, visited);
        total_score2 += dfs(map, i, j, 0, NULL);
      }
    }
  }
  printf("score: %d\n", total_score);
  printf("score2: %d\n", total_score2);
  free(visited);
  freeTmap(map);
  return 0;
}
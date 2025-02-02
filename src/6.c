#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum direction { UP, DOWN, LEFT, RIGHT };

struct position {
  int x;
  int y;
  enum direction dir;
};

struct gameMap {
  int rowLen;
  int colLen;
  struct position pos;
  char *map;
};

struct gameMap *loadGame(char *path) {
  struct gameMap *game = malloc(sizeof(struct gameMap));
  char buffer[BUFSIZ];
  FILE *f = fopen(path, "r");

  int positionFound = 0;
  game->map = NULL;
  game->rowLen = 0;
  game->colLen = 0;
  while (feof(f) == 0) {
    fgets(buffer, BUFSIZ, f);
    if (!game->rowLen)
      game->rowLen = strlen(buffer) - 1;
    game->map = realloc(game->map, game->rowLen * (game->colLen + 1));
    strncpy(game->map + game->colLen * game->rowLen, buffer, game->rowLen);
    if (!positionFound) {
      for (int j = 0; j < game->rowLen; j++) {
        if (buffer[j] == '^' || buffer[j] == 'v' || buffer[j] == '<' ||
            buffer[j] == '>') {
          game->pos.x = j;
          game->pos.y = game->colLen;
          switch (buffer[j]) {
          case '^':
            game->pos.dir = UP;
            break;
          case 'v':
            game->pos.dir = DOWN;
            break;
          case '<':
            game->pos.dir = LEFT;
            break;
          case '>':
            game->pos.dir = RIGHT;
            break;
          }
          positionFound = 1;
          break;
        }
      }
    }
    game->colLen++;
  }

  fclose(f);
  return game;
}

void destroyGame(struct gameMap *game) {
  free(game->map);
  free(game);
}

struct gameMap *dupGame(struct gameMap *game) {
  struct gameMap *newGame = malloc(sizeof(struct gameMap));
  newGame->rowLen = game->rowLen;
  newGame->colLen = game->colLen;
  newGame->pos = game->pos;
  newGame->map = malloc(newGame->rowLen * newGame->colLen);
  memcpy(newGame->map, game->map, newGame->rowLen * newGame->colLen);
  return newGame;
}

int hashPosition(struct gameMap *game) {
  return game->pos.x + game->pos.y * game->rowLen +
         game->pos.dir * game->rowLen * game->colLen;
}

int playGame(struct gameMap *game) {
  int distinctMoves = 0;
  int visitedLen = 0;
  int *visitedHash = calloc(game->rowLen * game->colLen * 4, sizeof(int));
  while (1) {
    int hash = hashPosition(game);
    if (visitedHash[hash]) {
      distinctMoves = -1;
      break;
    }
    visitedHash[hash] = 1;
    if (game->map[game->pos.y * game->rowLen + game->pos.x] != '*') {
      distinctMoves++;
    }
    game->map[game->pos.y * game->rowLen + game->pos.x] = '*';
    switch (game->pos.dir) {
    case UP:
      game->pos.y--;
      if (game->pos.y >= 0 &&
          game->map[game->pos.y * game->rowLen + game->pos.x] == '#') {
        game->pos.y++;
        game->pos.dir = RIGHT;
      }
      break;
    case DOWN:
      game->pos.y++;
      if (game->pos.y < game->colLen &&
          game->map[game->pos.y * game->rowLen + game->pos.x] == '#') {
        game->pos.y--;
        game->pos.dir = LEFT;
      }
      break;
    case LEFT:
      game->pos.x--;
      if (game->pos.x >= 0 &&
          game->map[game->pos.y * game->rowLen + game->pos.x] == '#') {
        game->pos.x++;
        game->pos.dir = UP;
      }
      break;
    case RIGHT:
      game->pos.x++;
      if (game->pos.x < game->rowLen &&
          game->map[game->pos.y * game->rowLen + game->pos.x] == '#') {
        game->pos.x--;
        game->pos.dir = DOWN;
      }
      break;
    }
    if (game->pos.x < 0 || game->pos.x >= game->rowLen || game->pos.y < 0 ||
        game->pos.y >= game->colLen) {
      break;
    }
  }
  free(visitedHash);
  return distinctMoves;
}

void printMap(struct gameMap *game) {
  for (int i = 0; i < game->colLen; i++) {
    for (int j = 0; j < game->rowLen; j++) {
      printf("%c", game->map[i * game->rowLen + j]);
    }
    printf("\n");
  }
}

int getLoops(struct gameMap *g) {
  int loops = 0;
  struct gameMap *game = dupGame(g);
  playGame(game);
  for (int i = 0; i < game->colLen; i++) {
    for (int j = 0; j < game->rowLen; j++) {
      if (game->map[i * game->rowLen + j] == '*' &&
          (g->pos.x != j || g->pos.y != i)) {
        struct gameMap *newGame = dupGame(g);
        newGame->map[i * game->rowLen + j] = '#';
        int res = playGame(newGame);
        if (res == -1) {
          newGame->map[i * game->rowLen + j] = 'O';
          loops++;
        }
        destroyGame(newGame);
      }
    }
  }
  destroyGame(game);
  return loops;
}

int main() {
  struct gameMap *game = loadGame("data/6.txt");
  struct gameMap *g = dupGame(game);
  int distinctMoves = playGame(g);
  destroyGame(g);
  printf("Distinct moves: %d\n", distinctMoves);
  int loops = getLoops(game);
  printf("Loops: %d\n", loops);

  destroyGame(game);
  return 0;
}
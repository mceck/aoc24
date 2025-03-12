#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFSIZE 4096

struct strList {
  char *str;
  struct strList *next;
};

struct pattern {
  char *name;
  int len;
};

struct patternV {
  struct pattern patterns[256];
  int size;
};

int comparePatterns(const void *a, const void *b) {
  return strcmp(((struct pattern *)a)->name, ((struct pattern *)b)->name);
}

int loadPatterns(char *path, struct patternV **_map, struct strList **_towels) {
  FILE *f = fopen(path, "r");
  char buffer[BUFFSIZE];
  char buffer2[BUFFSIZE];
  int maxLen = 0;

  fgets(buffer, BUFFSIZE, f);
  strcpy(buffer2, buffer);
  char *tok = strtok(buffer, ", ");
  while (tok) {
    int len = strlen(tok);
    if (tok[len - 1] == '\n') {
      len--;
    }
    if (len > maxLen) {
      maxLen = len;
    }
    tok = strtok(NULL, ", ");
  }

  struct patternV *map = calloc(maxLen, sizeof(struct patternV));

  tok = strtok(buffer2, ", ");
  while (tok) {
    int len = strlen(tok);
    if (tok[len - 1] == '\n') {
      tok[--len] = '\0';
    }
    int size = map[len - 1].size++;
    map[len - 1].patterns[size].name = strdup(tok);
    map[len - 1].patterns[size].len = len;
    tok = strtok(NULL, ", ");
  }

  fgets(buffer, BUFFSIZE, f);
  struct strList *towels = NULL;
  while (!feof(f)) {
    fgets(buffer, BUFFSIZE, f);
    struct strList *tow = malloc(sizeof(struct strList));
    int len = strlen(buffer);
    if (buffer[len - 1] == '\n') {
      buffer[len - 1] = '\0';
    }
    tow->str = strdup(buffer);
    tow->next = towels;
    towels = tow;
  }
  fclose(f);
  *_map = map;
  *_towels = towels;
  return maxLen;
}

int imin(int a, int b) { return a < b ? a : b; }

int64_t cache[BUFFSIZE];

int64_t checkTowel(struct patternV *map, int mapLen, char *towel,
                   char *towel0) {
  char *t = towel;
  int64_t res = 0;
  if (*t == '\0') {
    return 1;
  }
  for (int i = imin(mapLen - 1, strlen(t) - 1); i >= 0; i--) {
    struct pattern *p = map[i].patterns;
    for (int j = 0; j < map[i].size; j++) {
      if (strncmp(t, p[j].name, p[j].len) == 0) {
        int cacheIndex = (t + p[j].len - towel0);
        if (cache[cacheIndex] == 0) {
          continue;
        }
        if (cache[cacheIndex] > 0) {
          res += cache[cacheIndex];
          continue;
        }
        int64_t r = checkTowel(map, mapLen, t + p[j].len, towel0);
        if (r) {
          res += r;
        }
      }
    }
  }
  cache[t - towel0] = res;
  return res;
}

int main() {
  struct strList *towels;
  struct patternV *map;
  int mapLen = loadPatterns("data/19.txt", &map, &towels);
  int match = 0;
  int64_t match2 = 0;
  struct strList *t = towels;
  while (t) {
    for (int i = 0; i < BUFFSIZE; i++) {
      cache[i] = -1ll;
    }
    int64_t i = checkTowel(map, mapLen, t->str, t->str);
    if (i) {
      match++;
      match2 += i;
    } else {
    }
    t = t->next;
  }
  printf("Match: %d\n", match);
  printf("Match2: %lld\n", match2);
  return 0;
}
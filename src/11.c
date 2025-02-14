#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 128
#define HASH_SIZE 1000

struct stone {
  char *str;
  int len;
  struct stone *next;
};

struct llist {
  unsigned long long value;
  char *keyStr;
  int key;
  struct llist *next;
};

struct llist *cache[HASH_SIZE] = {0};

int hash(char *str, int key) {
  int hash = 0;
  for (int i = 0; i < strlen(str); i++) {
    hash = (hash * 31 + str[i]) % HASH_SIZE;
  }
  return (hash + key) % HASH_SIZE;
}

unsigned long long getCache(char *str, int key) {
  int h = hash(str, key);
  struct llist *current = cache[h];
  while (current) {
    if (current->key == key && strcmp(current->keyStr, str) == 0) {
      return current->value;
    }
    current = current->next;
  }
  return 0;
}

void addCache(char *str, int key, unsigned long long value) {
  int h = hash(str, key);
  struct llist *current = cache[h];
  while (current) {
    if (current->key == key && strcmp(current->keyStr, str) == 0) {
      current->value = value;
      return;
    }
    current = current->next;
  }
  struct llist *new = malloc(sizeof(struct llist));
  new->value = value;
  new->keyStr = strdup(str);
  new->key = key;
  new->next = cache[h];
  cache[h] = new;
}

void freeCache() {
  for (int i = 0; i < HASH_SIZE; i++) {
    struct llist *current = cache[i];
    while (current) {
      struct llist *next = current->next;
      free(current->keyStr);
      free(current);
      current = next;
    }
  }
  memset(cache, 0, sizeof(cache));
}

struct stone *load(char *path) {
  char str[256];
  FILE *f = fopen(path, "r");
  fgets(str, BUFFER_SIZE, f);
  fclose(f);
  char *token = strtok(str, " ");
  struct stone *head = malloc(sizeof(struct stone));
  struct stone *current = head;
  while (token) {
    current->str = strdup(token);
    current->len = strlen(token);
    token = strtok(NULL, " ");
    if (token) {
      current->next = malloc(sizeof(struct stone));
      current = current->next;
    }
  }
  current->next = NULL;

  return head;
}

void destroyStones(struct stone *l) {
  while (l) {
    struct stone *next = l->next;
    free(l->str);
    free(l);
    l = next;
  }
}

char *multStone(char *a, int b) {
  int carry = 0, i = 0, product;
  char *result = calloc(BUFFER_SIZE, sizeof(char));
  int len = strlen(a);
  for (int j = len - 1; j >= 0; j--) {
    product = (a[j] - '0') * b + carry;
    result[i++] = product % 10 + '0';
    carry = product / 10;
  }
  while (carry) {
    result[i++] = carry % 10 + '0';
    carry /= 10;
  }
  for (int j = 0; j < i / 2; j++) {
    char tmp = result[j];
    result[j] = result[i - j - 1];
    result[i - j - 1] = tmp;
  }
  return result;
}

unsigned long long blink(char *str, int blinks) {
  if (blinks == 0) {
    return 1;
  }
  unsigned long long count = getCache(str, blinks);
  if (count) {
    return count;
  }
  int len = strlen(str);
  if (str[0] == '0') {
    count += blink("1", blinks - 1);
  } else if (len % 2 == 0) {
    int p = len / 2;
    char *first = calloc(p + 1, sizeof(char));
    char *second = calloc(p + 1, sizeof(char));
    int lf = 0, ls = 0;
    for (int i = 0; i < p; i++) {
      if (lf > 0 || str[i] != '0') {
        first[lf++] = str[i];
      }
      if (ls > 0 || str[i + p] != '0') {
        second[ls++] = str[i + p];
      }
    }
    if (lf == 0) {
      first[lf++] = '0';
    }
    if (ls == 0) {
      second[ls++] = '0';
    }
    first[lf] = '\0';
    second[ls] = '\0';
    count += blink(first, blinks - 1);
    count += blink(second, blinks - 1);
    free(first);
    free(second);
  } else {
    char *mult = multStone(str, 2024);
    count += blink(mult, blinks - 1);
    free(mult);
  }
  addCache(str, blinks, count);
  return count;
}

int main() {
  struct stone *head = load("data/11.txt");

  unsigned long long count = 0;
  struct stone *l = head;
  while (l) {
    count += blink(l->str, 25);
    l = l->next;
  }
  printf("Result1: %llu\n", count);
  freeCache();

  count = 0;
  l = head;
  while (l) {
    count += blink(l->str, 75);
    l = l->next;
  }
  printf("Result2: %llu\n", count);
  freeCache();

  destroyStones(l);
}
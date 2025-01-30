#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 20000

int until(char *buffer, char *match, int start) {
  if (start < 0) {
    return -1;
  }
  int i = start;
  while (buffer[i] != '\0') {
    int j = 0;
    while (buffer[i + j] == match[j] && match[j] != '\0') {
      j++;
    }
    if (match[j] == '\0') {
      return i + j;
    }
    i++;
  }
  return -1;
}

int getInt(char *buffer, int *num, int start) {
  int i = start;
  *num = 0;
  while (buffer[i] >= '0' && buffer[i] <= '9' && buffer[i] != '\0') {
    *num = *num * 10 + buffer[i] - '0';
    i++;
  }
  return i;
}

int just(char *buffer, char c, int index) { return buffer[index] == c; }

int parseExpr(char *buffer) {
  int i = 0;
  int num = 0;
  while ((i = until(buffer, "mul(", i)) != -1) {
    int a, b;
    i = getInt(buffer, &a, i);
    if (just(buffer, ',', i)) {
      i++;
    } else {
      continue;
    }
    i = getInt(buffer, &b, i);
    if (just(buffer, ')', i)) {
      i++;
    } else {
      continue;
    }
    num += a * b;
  }
  return num;
}

int parseExpr2(char *buffer) {
  int i = 0;
  int num = 0;
  while (1) {
    int k = until(buffer, "don't()", i);
    i = until(buffer, "mul(", i);
    if (i == -1)
      break;
    if (k != -1 && k < i) {
      i = until(buffer, "do()", i);
      continue;
    }

    int a, b;
    i = getInt(buffer, &a, i);
    if (just(buffer, ',', i)) {
      i++;
    } else {
      continue;
    }
    i = getInt(buffer, &b, i);
    if (just(buffer, ')', i)) {
      i++;
    } else {
      continue;
    }
    num += a * b;
  }
  return num;
}

int main() {
  char buffer[BUFFER_SIZE];
  FILE *f = fopen("data/3.txt", "r");
  fread(buffer, 1, BUFFER_SIZE, f);
  fclose(f);
  int res = parseExpr(buffer);
  printf("Mult: %d\n", res);
  res = parseExpr2(buffer);
  printf("Mult v2: %d\n", res);

  return 0;
}
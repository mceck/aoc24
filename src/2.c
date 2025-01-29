#include <stdio.h>
#include <stdlib.h>

int readLine(FILE *f, int *arr) {
  int i = 0;
  while (1) {
    int num;
    fscanf(f, "%d", &num);
    arr[i++] = num;
    char sep = fgetc(f);
    if (sep == '\n' || sep == EOF) {
      break;
    }
  }
  return i;
}

void filterIndex(int j, int *input, int *output, int len) {
  int k = 0;
  for (int i = 0; i < len; i++) {
    if (i != j) {
      output[k++] = input[i];
    }
  }
}

int checkRow(int *buffer, int len, int jolly) {
  if (len < 2) {
    return 1;
  }
  int dir = buffer[1] > buffer[0] ? 1 : -1;
  for (int i = 1; i < len; i++) {
    int level = buffer[i];
    int prev = buffer[i - 1];

    if (level == prev || abs(level - prev) > 3 || (dir == 1 && level < prev) ||
        (dir == -1 && level > prev)) {
      if (!jolly) {
        return 0;
      }
      int partial[32];
      filterIndex(i, buffer, partial, len);
      if (checkRow(partial, len - 1, jolly - 1)) {
        return 1;
      }
      filterIndex(i - 1, buffer, partial, len);
      if (checkRow(partial, len - 1, jolly - 1)) {
        return 1;
      }
      if (i == 2) {
        filterIndex(0, buffer, partial, len);
        return checkRow(partial, len - 1, jolly - 1);
      }
      return 0;
    }
  }
  return 1;
}

int checkSafeLevels(int jolly) {
  FILE *f = fopen("data/2.txt", "r");
  int total = 0;
  int buffer[32];

  while (!feof(f)) {
    int len = readLine(f, buffer);
    if (len == 0)
      continue;
    if (checkRow(buffer, len, jolly))
      total++;
  }
  fclose(f);
  return total;
}

int main() {
  int safeLevels = checkSafeLevels(0);
  printf("Safe levels: %d\n", safeLevels);
  safeLevels = checkSafeLevels(1);
  printf("Safe levels v2: %d\n", safeLevels);
  return 0;
}
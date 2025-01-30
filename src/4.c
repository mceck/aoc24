#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int count(char *buffer, char *match) {
  int i = 0;
  int count = 0;
  while ((i = until(buffer, match, i)) != -1) {
    count++;
  }
  return count;
}

char *loadFile(char *path) {
  FILE *f = fopen(path, "r");
  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *buffer = malloc(size + 1);
  fread(buffer, 1, size, f);
  fclose(f);
  return buffer;
}

int rowSize(char *buffer) {
  int i = 0;
  while (buffer[i] != '\n' && buffer[i] != '\0') {
    i++;
  }
  return i;
}

int getXmas(char *buffer) {
  int len = strlen(buffer);
  int rowLen = rowSize(buffer);
  char *rowBuffer = malloc(rowLen + 1);
  int res = 0;

  int row = 0;
  for (int row = 0; row < rowLen; row++) {
    // row
    memcpy(rowBuffer, buffer + row * (rowLen + 1), rowLen);
    rowBuffer[rowLen] = '\0';
    int c = count(rowBuffer, "XMAS");
    c += count(rowBuffer, "SAMX");
    res += c;
  }
  // columns
  for (int i = 0; i < rowLen; i++) {
    for (int j = 0; j < rowLen; j++) {
      rowBuffer[j] = buffer[j * (rowLen + 1) + i];
    }
    rowBuffer[rowLen] = '\0';
    int c = count(rowBuffer, "XMAS");
    c += count(rowBuffer, "SAMX");
    res += c;
  }

  // diagonals from top-left to bottom direction right
  for (int i = 0; i < rowLen; i++) {
    int j = 0;
    while (i + j < rowLen && j < rowLen) {
      rowBuffer[j] = buffer[(i + j) * (rowLen + 1) + j];
      j++;
    }
    rowBuffer[j] = '\0';
    int c = count(rowBuffer, "XMAS");
    c += count(rowBuffer, "SAMX");
    res += c;
  }

  // diagonals from top-right to bottom direction left
  for (int i = 0; i < rowLen; i++) {
    int j = 0;
    while (i + j < rowLen && j < rowLen) {
      rowBuffer[j] = buffer[(i + j) * (rowLen + 1) + rowLen - j - 1];
      j++;
    }
    rowBuffer[j] = '\0';
    int c = count(rowBuffer, "XMAS");
    c += count(rowBuffer, "SAMX");
    res += c;
  }

  // diagonals from top-left to right direction right, skipping the main
  for (int i = 1; i < rowLen; i++) {
    int j = 0;
    while (i + j < rowLen && j < rowLen) {
      rowBuffer[j] = buffer[j * (rowLen + 1) + i + j];
      j++;
    }
    rowBuffer[j] = '\0';
    int c = count(rowBuffer, "XMAS");
    c += count(rowBuffer, "SAMX");
    res += c;
  }

  // diagonals from top-right to left direction left, skipping the main diagonal
  for (int i = 1; i < rowLen; i++) {
    int j = 0;
    while (i + j < rowLen && j < rowLen) {
      rowBuffer[j] = buffer[j * (rowLen + 1) + rowLen - i - j - 1];
      j++;
    }
    rowBuffer[j] = '\0';
    int c = count(rowBuffer, "XMAS");
    c += count(rowBuffer, "SAMX");
    res += c;
  }
  free(rowBuffer);
  return res;
}

int checkXmas(char *buffer, int i, int j, int rowLen) {
  return buffer[i * (rowLen + 1) + j] == 'A' &&
         ((buffer[(i - 1) * (rowLen + 1) + j - 1] == 'M' &&
           buffer[(i + 1) * (rowLen + 1) + j + 1] == 'S' &&
           buffer[(i - 1) * (rowLen + 1) + j + 1] == 'M' &&
           buffer[(i + 1) * (rowLen + 1) + j - 1] == 'S') ||
          (buffer[(i - 1) * (rowLen + 1) + j - 1] == 'S' &&
           buffer[(i + 1) * (rowLen + 1) + j + 1] == 'M' &&
           buffer[(i - 1) * (rowLen + 1) + j + 1] == 'S' &&
           buffer[(i + 1) * (rowLen + 1) + j - 1] == 'M') ||
          (buffer[(i - 1) * (rowLen + 1) + j - 1] == 'M' &&
           buffer[(i + 1) * (rowLen + 1) + j + 1] == 'S' &&
           buffer[(i - 1) * (rowLen + 1) + j + 1] == 'S' &&
           buffer[(i + 1) * (rowLen + 1) + j - 1] == 'M') ||
          (buffer[(i - 1) * (rowLen + 1) + j - 1] == 'S' &&
           buffer[(i + 1) * (rowLen + 1) + j + 1] == 'M' &&
           buffer[(i - 1) * (rowLen + 1) + j + 1] == 'M' &&
           buffer[(i + 1) * (rowLen + 1) + j - 1] == 'S'));
}

int getXmas2(char *buffer) {
  int len = strlen(buffer);
  int rowLen = rowSize(buffer);
  int res = 0;

  for (int i = 1; i < rowLen - 1; i++) {
    for (int j = 1; j < rowLen - 1; j++) {
      if (checkXmas(buffer, i, j, rowLen)) {
        res++;
      }
    }
  }

  return res;
}

int main() {
  char *buffer = loadFile("data/4.txt");
  int res = getXmas(buffer);
  printf("Total XMAS: %d\n", res);
  res = getXmas2(buffer);
  printf("Total X-MAS: %d\n", res);
  free(buffer);
  return 0;
}
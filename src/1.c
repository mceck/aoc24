#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

typedef struct inputData inputData;

struct inputData {
  int a[BUFFER_SIZE];
  int b[BUFFER_SIZE];
  int size;
};

int intcmp(const void *a, const void *b) { return *(int *)a - *(int *)b; }

inputData *readInput() {
  FILE *f = fopen("data/1.txt", "r");
  inputData *data = malloc(sizeof(inputData));
  char a[8];
  char b[8];

  for (data->size = 0; feof(f) == 0; data->size++) {
    // split by ,
    fscanf(f, "%[^,],%s\n", a, b);
    data->a[data->size] = atoi(a);
    data->b[data->size] = atoi(b);
  }
  fclose(f);

  qsort(data->a, data->size, sizeof(int), &intcmp);
  qsort(data->b, data->size, sizeof(int), &intcmp);
  return data;
}

int sumDistances(inputData *data) {
  int sum = 0;
  for (int i = 0; i < data->size; i++) {
    int dist = data->a[i] - data->b[i];
    sum += dist > 0 ? dist : -dist;
  }
  return sum;
}

int count(int val, int *v, int size) {
  int count = 0;
  for (int i = 0; i < size; i++) {
    if (v[i] == val) {
      count++;
    }
  }
  return count;
}

int sumSimilarity(inputData *data) {
  int sum = 0;
  for (int i = 0; i < data->size; i++) {
    sum += (data->a[i] * count(data->a[i], data->b, data->size));
  }
  return sum;
}

int main() {
  inputData *data = readInput();
  int sum = sumDistances(data);
  printf("Sum of distances: %d\n", sum);
  sum = sumSimilarity(data);
  printf("Similarity: %d\n", sum);
  free(data);
  return 0;
}
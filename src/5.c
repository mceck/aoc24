#include <stdio.h>
#include <stdlib.h>

#define VSIZ 32

typedef struct rule rule;
typedef struct inputData inputData;
typedef struct listVector listVector;

struct rule {
  int x;
  int ls;
  rule *next;
};

struct listVector {
  int vector[VSIZ];
  struct listVector *next;
};

struct inputData {
  rule *rules;
  listVector *vectors;
};

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

inputData *loadRules(char *path) {
  FILE *f = fopen(path, "r");
  inputData *data = malloc(sizeof(inputData));
  data->rules = NULL;
  data->vectors = NULL;
  int len = 0;
  char buffer[256];
  while (feof(f) == 0) {
    fgets(buffer, 256, f);
    int a, b;
    int x = sscanf(buffer, "%d|%d", &a, &b);
    if (x != 2) {
      break;
    }
    rule *r = malloc(sizeof(rule));
    r->x = a;
    r->ls = b;
    r->next = data->rules;
    data->rules = r;
    len++;
  }

  while (feof(f) == 0) {
    fgets(buffer, 256, f);
    listVector *v = malloc(sizeof(listVector));
    int i = 0, k = 0;
    while (1) {
      int num;
      k = getInt(buffer, &num, k);
      v->vector[i] = num;
      i++;
      if (!just(buffer, ',', k)) {
        break;
      }
      k++;
    }
    v->next = data->vectors;
    data->vectors = v;
  }
  fclose(f);
  return data;
}

int indexOf(int *vector, int x) {
  int i = 0;
  while (vector[i] != 0) {
    if (vector[i] == x) {
      return i;
    }
    i++;
  }
  return -1;
}

int evaluate(rule *r, int *vector) {
  while (r != NULL) {
    int ix = indexOf(vector, r->x);
    int ils = indexOf(vector, r->ls);
    if (ix >= 0 && ils >= 0 && ix > ils) {
      return 0;
    }

    r = r->next;
  }
  return 1;
}

int midValue(int *vector) {
  int len = 0;
  while (vector[len] != 0) {
    len++;
  }
  return vector[len / 2];
}

void fixVector(rule *r0, int *vector) {
  rule *r = r0;
  while (r != NULL) {
    int ix = indexOf(vector, r->x);
    int ils = indexOf(vector, r->ls);
    if (ix >= 0 && ils >= 0 && ix > ils) {
      int temp = vector[ix];
      vector[ix] = vector[ils];
      vector[ils] = temp;
      r = r0;
    } else {
      r = r->next;
    }
  }
}

void cleanup(inputData *data) {
  while (data->rules != NULL) {
    rule *temp = data->rules;
    data->rules = data->rules->next;
    free(temp);
  }
  while (data->vectors != NULL) {
    listVector *temp = data->vectors;
    data->vectors = data->vectors->next;
    free(temp);
  }
  free(data);
}

int main() {
  inputData *data = loadRules("data/5.txt");
  rule *r = data->rules;
  listVector *v = data->vectors;
  int sum = 0, sum2 = 0;
  while (v != NULL) {
    int *vector = v->vector;
    if (evaluate(r, vector)) {
      sum += midValue(vector);
    } else {
      fixVector(r, vector);
      sum2 += midValue(vector);
    }
    v = v->next;
  }
  printf("Sum: %d\n", sum);
  printf("Sum2: %d\n", sum2);

  cleanup(data);
  return 0;
}
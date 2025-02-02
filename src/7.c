#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct testCase {
  double n;
  int len;
  double *values;
};

struct testList {
  int len;
  struct testCase *tests;
};

struct dList {
  double v;
  struct dList *next;
};

struct testList *loadTests(char *path) {
  FILE *f = fopen(path, "r");
  struct testCase *tests = NULL;
  char buffer[256];
  int len = 0;
  while (feof(f) == 0) {
    fgets(buffer, 256, f);
    tests = realloc(tests, ++len * sizeof(struct testCase));
    struct testCase *test = &tests[len - 1];
    test->values = NULL;
    char nbuf[128];
    int i = 0, j = 0, k = 0;
    do {
      if (buffer[i] == ':') {
        nbuf[j] = '\0';
        test->n = atof(nbuf);
        j = 0;
      } else if (buffer[i] >= '0' && buffer[i] <= '9') {
        nbuf[j++] = buffer[i];
      } else if (j > 0) {
        nbuf[j] = '\0';
        test->values = realloc(test->values, ++k * sizeof(double));
        test->values[k - 1] = atof(nbuf);
        j = 0;
      }
      i++;
    } while (buffer[i - 1] != '\n' && buffer[i - 1] != '\0');
    test->len = k;
  }
  fclose(f);
  struct testList *list = malloc(sizeof(struct testList));
  list->len = len;
  list->tests = tests;

  return list;
}

void destroyTests(struct testList *tests) {
  for (int i = 0; i < tests->len; i++) {
    free(tests->tests[i].values);
  }
  free(tests->tests);
  free(tests);
}

double concatDouble(double a, double b) {
  double c = b;
  while (c >= 1) {
    c /= 10.0;
    a *= 10.0;
  }
  return a + b;
}

int check(struct testCase *test, int concat) {
  struct dList *sums = NULL;
  for (int i = 0; i < test->len; i++) {
    if (sums == NULL) {
      sums = malloc(sizeof(struct dList));
      sums->v = test->values[i];
      sums->next = NULL;
    } else {
      struct dList *s = sums;
      while (s != NULL) {
        // multiply
        struct dList *m = malloc(sizeof(struct dList));
        m->v = s->v * test->values[i];
        m->next = sums;
        sums = m;
        // concat
        if (concat) {
          m = malloc(sizeof(struct dList));
          m->v = concatDouble(s->v, test->values[i]);
          m->next = sums;
          sums = m;
        }
        // add
        s->v += test->values[i];
        s = s->next;
      }
    }
  }
  int ok = 0;
  struct dList *s = sums;
  while (s != NULL) {
    if (s->v == test->n) {
      ok++;
    }
    struct dList *d = s;
    s = s->next;
    free(d);
  }
  return ok;
}

int main() {
  struct testList *tests = loadTests("data/7.txt");
  double total = 0;
  double total2 = 0;
  struct testCase *t = tests->tests;
  for (int i = 0; i < tests->len; i++) {
    int pass = check(&t[i], 0);
    if (pass > 0) {
      total += t[i].n;
    }
    pass = check(&t[i], 1);
    if (pass > 0) {
      total2 += t[i].n;
    }
  }
  printf("Total: %.0f\n", total);
  printf("Total2: %.0f\n", total2);
  destroyTests(tests);
  return 0;
}
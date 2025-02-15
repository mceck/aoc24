#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct op {
  double ax;
  double ay;
  double bx;
  double by;
  double k;
  double j;
  struct op *next;
};

int isInteger(double num) {
  const double EPSILON = 1e-9;
  return num >= 0 && fabs(num - round(num)) < EPSILON;
}

double solve(double ax, double ay, double bx, double by, double k, double j) {
  double denominator = ax * by - ay * bx;

  if (fabs(denominator) < 1e-9) {
    return 0;
  }

  double m = (k * by - j * bx) / denominator;
  double n = (j * ax - k * ay) / denominator;

  if (isInteger(m) && isInteger(n)) {
    return 3 * m + n;
  }
  return 0;
}

struct op *loadOps(char *path) {
  FILE *file = fopen(path, "r");
  struct op *head = NULL;
  struct op *tail = NULL;
  while (feof(file) == 0) {
    struct op *op = calloc(1, sizeof(struct op));
    fscanf(file,
           "Button A: X+%lf, Y+%lf\nButton B: X+%lf, Y+%lf\n"
           "Prize: X=%lf, Y=%lf\n\n",
           &op->ax, &op->ay, &op->bx, &op->by, &op->k, &op->j);
    if (head == NULL) {
      head = op;
    }
    if (tail != NULL) {
      tail->next = op;
    }
    tail = op;
  }
  fclose(file);
  return head;
}

void destroyOps(struct op *ops) {
  while (ops != NULL) {
    struct op *next = ops->next;
    free(ops);
    ops = next;
  }
}

int main() {
  struct op *ops = loadOps("data/13.txt");

  struct op *op = ops;
  double total = 0;
  while (op != NULL) {
    total += solve(op->ax, op->ay, op->bx, op->by, op->k, op->j);
    op = op->next;
  }
  printf("Total: %.0lf\n", total);

  total = 0;
  op = ops;
  while (op != NULL) {
    total += solve(op->ax, op->ay, op->bx, op->by, op->k + 10000000000000,
                   op->j + 10000000000000);
    op = op->next;
  }
  printf("Total2: %.0lf\n", total);

  destroyOps(ops);
  return 0;
}
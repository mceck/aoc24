#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 20000

int main() {
  char fs[BUFFER_SIZE];
  FILE *file = fopen("data/9.txt", "r");
  fgets(fs, BUFFER_SIZE, file);
  int len = strlen(fs);
  int k = 0;
  double chksum = 0;
  int tail = len % 2 == 0 ? len - 2 : len - 1;
  int tailV = fs[tail] - '0';
  int i;
  for (i = 0; i < tail; i++) {
    int v = fs[i] - '0';
    if (i % 2 == 0) {
      // file block
      while (v--) {
        chksum += (k * (i / 2));
        k++;
      }
    } else {
      // free block
      while (v-- && tailV) {
        chksum += (k * (tail / 2));
        k++;
        tailV--;
        if (tailV == 0 && tail >= 2) {
          tail -= 2;
          tailV = fs[tail] - '0';
          if (tail < i)
            break;
        }
      }
    }
  }
  while (tailV && tail == i) {
    chksum += (k * (tail / 2));
    k++;
    tailV--;
  }
  printf("chk %.0f\n", chksum);
  return 0;
}
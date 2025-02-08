#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 20000

double fsfrag(char *fs) {
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
  return chksum;
}

double fsfrag2(char *fs) {
  int len = strlen(fs);
  int k = 0;

  int expanded[BUFFER_SIZE * 9];
  for (int i = 0; i < len; i++) {
    int v = fs[i] - '0';
    if (i % 2 == 0) {
      // file block
      while (v--) {
        expanded[k++] = i / 2;
      }
    } else {
      // free block
      while (v--) {
        expanded[k++] = -1;
      }
    }
  }
  int expLen = k--;
  int evalLimit = expanded[k] + 1;
  while (k > 0) {
    int kCount = 1;
    if (expanded[k] != -1 && evalLimit > expanded[k]) {
      while (expanded[k - kCount] == expanded[k]) {
        kCount++;
      }
      for (int i = 0; i < k; i++) {
        if (expanded[i] == -1) {
          int fCount = 1;
          while (expanded[i + fCount] == -1) {
            fCount++;
          }
          if (fCount >= kCount) {
            evalLimit = expanded[k];
            for (int j = 0; j < kCount; j++) {
              expanded[i + j] = expanded[k - j];
              expanded[k - j] = -1;
            }
            break;
          } else {
            i += fCount;
          }
        }
      }
    }
    k -= kCount;
  }

  double chksum = 0;
  for (int i = 0; i < expLen; i++) {
    if (expanded[i] != -1)
      chksum += expanded[i] * i;
  }
  return chksum;
}

int main() {
  char fs[BUFFER_SIZE];
  FILE *file = fopen("data/9.txt", "r");
  fgets(fs, BUFFER_SIZE, file);
  fclose(file);
  double chksum = fsfrag(fs);
  printf("chk %.0f\n", chksum);
  chksum = fsfrag2(fs);
  printf("chk2 %.0f\n", chksum);
  return 0;
}
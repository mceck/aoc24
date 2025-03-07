#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum opcodes {
  ADV = 0,
  BXL = 1,
  BST = 2,
  JNZ = 3,
  BXC = 4,
  OUT = 5,
  BDV = 6,
  CDV = 7
};

uint64_t A = 0;
uint64_t B = 0;
uint64_t C = 0;
char p_stdout[BUFSIZ];
uint64_t p_stdout_ptr = 0;

uint64_t i_ptr = 0;
uint64_t prg_len = 0;

char *loadProgram(char *path) {
  FILE *file = fopen(path, "r");
  fscanf(file, "Register A: %lld\n", &A);
  fscanf(file, "Register B: %lld\n", &B);
  fscanf(file, "Register C: %lld\n\n", &C);
  char buffer[BUFSIZ];
  fscanf(file, "Program: %s\n", buffer);
  fclose(file);
  int len = strlen(buffer);
  prg_len = (len / 2) + 1;
  char *program = malloc(prg_len + 1);
  for (int i = 0; i < len; i += 2) {
    program[i / 2] = (buffer[i]);
  }
  program[prg_len] = '\0';
  p_stdout[0] = '\0';
  p_stdout_ptr = 0;
  return program;
}

uint64_t resolveComboOperand(char operand) {
  switch (operand) {
  case 4:
    return A;
  case 5:
    return B;
  case 6:
    return C;
  default:
    return (uint64_t)operand;
  }
}

void prgLoop(char *program) {
  char operation = *(program + i_ptr++) - '0';
  uint64_t litOperand = (uint64_t)*(program + i_ptr++) - '0';
  uint64_t comboOperand = resolveComboOperand(litOperand);
  switch (operation) {
  case ADV:
    A = A >> comboOperand;
    break;
  case BXL:
    B = B ^ litOperand;
    break;
  case BST:
    B = comboOperand & 0x7;
    break;
  case JNZ:
    if (A == 0)
      break;
    i_ptr = litOperand;
    break;
  case BXC:
    B = B ^ C;
    break;
  case OUT:
    p_stdout[p_stdout_ptr++] = (comboOperand & 0x7) + '0';
    p_stdout[p_stdout_ptr] = '\0';
    break;
  case BDV:
    B = A >> comboOperand;
    break;
  case CDV:
    C = A >> comboOperand;
    break;
  default:
    printf("Unknown opcode: %d\n", operation);
    exit(1);
  }
}

int main() {
  char *program = loadProgram("data/17.txt");
  printf("Program: %s\n", program);
  while (i_ptr < prg_len) {
    prgLoop(program);
  }
  printf("OUT: %s\n", p_stdout);
  free(program);

  //   printf("\n");
  //   program = loadProgram("data/17.txt");
  //   uint64_t bkB = B;
  //   uint64_t bkC = C;
  //   A = 0;
  //   uint64_t bkA = A;
  //   printf("Program: %s\n", program);
  //   int err;
  //   while (1) {
  //     err = 0;
  //     while (i_ptr < prg_len) {
  //       prgLoop(program);
  //       if (p_stdout_ptr &&
  //           p_stdout[p_stdout_ptr - 1] != program[p_stdout_ptr - 1]) {
  //         err = 1;
  //         break;
  //       }
  //     }
  //     if (!err && strcmp(p_stdout, program) == 0) {
  //       printf("Found match! %llu\n", bkA);
  //       break;
  //     }
  //     i_ptr = 0;
  //     p_stdout_ptr = 0;
  //     A = ++bkA;
  //     B = bkB;
  //     C = bkC;

  //     if (A == 0) {
  //       printf("No match found\n");
  //       break;
  //     }
  //   }
  //   free(program);
  return 0;
}
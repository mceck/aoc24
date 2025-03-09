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

// B = A & 0b111
// B = B ^ 0b011
// C = A >> B
// B = B ^ C
// B = B ^ 0b011
// A = A >> 3
// print B
// jmpz
u_int64_t computePartA(char *program) {
  u_int64_t res = 0;
  while (1) {
    A = res;
    B = 0;
    C = 0;
    i_ptr = 0;
    p_stdout_ptr = 0;
    p_stdout[0] = '\0';
    int err = 0;
    int z = 1;
    for (int p = 0; p < 8; p++) {
      for (int k = 0; k < prg_len; k += 2) {
        prgLoop(program);
      }
      char out = p_stdout[p];
      if (out != program[p]) {
        err = 1;
        break;
      }
    }
    if (!err) {
      break;
    }
    res++;
  }
  printf("hex: %llx\n", res);
  printf("partial out %s\n\n", p_stdout);
  return res;
}

int main() {
  char *program = loadProgram("data/17.txt");
  printf("Program: %s len: %lld\n", program, prg_len);
  while (i_ptr < prg_len) {
    prgLoop(program);
  }
  printf("OUT:     %s\n", p_stdout);
  free(program);

  printf("\n");
  program = loadProgram("data/17.txt");

  // find the first part, 8 digits
  u_int64_t cA = computePartA(program);
  free(program);

  // bruteforcing the rest
  program = loadProgram("data/17.txt");
  printf("Program: %s\n", program);
  for (uint64_t a = 0; a < 0x100000; a++) {
    u_int64_t nA = (a << 28) | cA;
    A = nA;
    i_ptr = 0;
    p_stdout_ptr = 0;
    p_stdout[0] = '\0';

    while (i_ptr < prg_len) {
      prgLoop(program);
    }
    if (strcmp(p_stdout, program) == 0) {
      printf("OUT:     %s\n", p_stdout);
      printf("A: %llu\n", nA);
      break;
    }
  }
  free(program);
  return 0;
}

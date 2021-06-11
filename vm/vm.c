#include <stdio.h>

extern int Backdoor(int);

int main(int argc, char* argv) {
  int i;
  puts("Hello world\r\n");

  i = Backdoor(6);
  printf("Selection Len (asm): %d\r\n", i);

  return 0;
}




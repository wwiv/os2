#include <stdio.h>

extern int Ret4();
extern int RetP(int);
extern int Backdoor(int);

// MAGIC number to send to backdoor api
#define BDOOR_MAGIC 0x564D5868;

// Low-bandwidth backdoor port number 
// for the IN/OUT interface.
#define BDOOR_PORT 0x5658;
#define BDOOR_MAGIC_T ~BDOOR_MAGIC
int b(int cmd) {
  long result = 1;
  long t = BDOOR_MAGIC_T;
  _asm {
       push edx
       push ecx
       push ebx
       mov ecx, cmd
       mov eax, 564D5868h
       mov dx, 5658h
       in eax, dx
       add eax, 21
       mov result, eax
       mov [result], 22
       pop ebx
       pop ecx
       pop edx
       }
   return result;
}

int main(int argc, char* argv) {
  int i;
  puts("Hello world\r\n");

  i = RetP(123);
  printf("RetP: %d\r\n", i);

  i = Backdoor(6);
  printf("Selection Len (asm): %d\r\n", i);

  return 0;
}




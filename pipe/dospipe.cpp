#include <ctype.h>
#include <dos.h>
#include <fcntl.h>
#include <io.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define ENABLE_LOG

#define PIPE_BUFFER_SIZE 4000

static void log(const char* msg, ...) {
#ifdef ENABLE_LOG
  va_list argptr;
  va_start(argptr, msg);
  vfprintf(stderr, msg, argptr);
  va_end(argptr);
  fprintf(stderr, "\r\n");
  fflush(stderr);
#endif
}

static void os_yield() {
  log("os_yield");
  union _REGS r;
  r.x.ax = 0x1680;
  _int86(0x2f, &r, &r);
}

int main(int argc, char** argv) {
  int f = _open("\\PIPE\\FOO", _O_RDWR);
  if (f < 0) {
    log("Unable to open pipe");
    return 1;
  } else {
    log("Opened Pipe");
  }
  
  int i = 0;
  while (1) {
    char buf;
    int read = _read(f, &buf, 1);
    if (read < 1) {
      os_yield();
      fprintf(stderr, ".");
    } else {
      log("Read: [%c] : [%d]", buf, buf);
    }
  }

  log("Closing Pipe!\n");
  close(f);
  return 0;
}

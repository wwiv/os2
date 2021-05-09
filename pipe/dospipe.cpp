#include "pipe.h"

#include <conio.h>
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

static void outch(int ch) {
  fprintf(stdout, "%c", ch);
  if (ch == '\r') {
    fprintf(stdout, "\n");
  }
  fflush(stdout);
}

int main(int, char**) {
  int f = _open("\\PIPE\\FOO", _O_RDWR | _O_BINARY);
  if (f < 0) {
    log("Unable to open pipe");
    return 1;
  } else {
    log("Opened Pipe");
  }
  
  int i = 0;
  for (;;) {
    char buf;
    int num_waiting = DosPeekNmPipe(f);
    if (num_waiting < 0) {
      break;
    }
    if (_kbhit()) {
      int ch = getch();
      if (ch == 27) {
	log("Exit signaled.");
	break;
      }
      _write(f, &ch, 1);
      outch(ch);
    }
    if (!num_waiting) {
      os_yield();
      if ((i++ % 1000) == 0) {
	fprintf(stdout, ".");
	fflush(stdout);
      }
      continue;
    }
    int read = _read(f, &buf, 1);
    if (read < 1) {
      break;
    } if (read == 0) {
      log("-");
    } else {
      outch(buf);
    }
  }

  log("Closing Pipe!\n");
  close(f);
  return 0;
}

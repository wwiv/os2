#include "pipe.h"
#include "util.h"
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

int main(int, char**) {
  log("PIPECLNT.EXE: Opening: \\PIPE\\WWIV1");
  Pipe __far * pipe = new __far Pipe("\\PIPE\\WWIV1");
  if (!pipe->is_open()) {
    log("Unable to open pipe");
    return 1;
    os_yield();
  } else {
    log("Opened Pipe");
  }
  
  int i = 0;
  for (;;) {
    char buf;
    int num_waiting = pipe->peek();
    if (num_waiting < 0) {
      log("num_waiting < 0: [%d]", num_waiting);
      break;
    }
    if (_kbhit()) {
      int ch = getch();
      if (ch == 27) {
	log("Exit signaled.");
	break;
      }
      pipe->write(ch);
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
    log("CALLING READ");
    buf = pipe->read();
    if (buf < 1) {
      log("buf < 1: %d", buf);
      break;
    } if (buf == 0) {
      log("-");
    } else {
      outch(buf);
    }
  }

  log("Closing Pipe!");
  pipe->close();
  log("Pipe Closed!");
  delete pipe;
  pipe = NULL;
  return 0;
}

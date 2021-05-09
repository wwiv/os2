#define INCL_WHATEVER
#define INCL_DOSDEVIOCTL
#define INCL_DOSPROCESS
#include <os2.h>

#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENABLE_LOG

static constexpr int PIPE_BUFFER_SIZE = 4000;

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
  DosSleep(250);
}

static HFILE create_pipe(const char *name) {
  char pipe_name[200];
  sprintf(pipe_name, "\\PIPE\\%s", name);
  log("Dos Create Pipe name : %s", pipe_name);
  HFILE hPipe;
  auto rc = DosCreateNPipe((const unsigned char*) pipe_name,
			   &hPipe,
			   NP_ACCESS_DUPLEX,
			   NP_NOWAIT | NP_TYPE_BYTE | NP_READMODE_BYTE | 255,
			   PIPE_BUFFER_SIZE,
			   PIPE_BUFFER_SIZE,
			   0); // 0 == No Wait.
  log("Dos Create Pipe returns : %d",rc);
  do {
    rc = DosConnectNPipe(hPipe);
    if (rc != 0) {
      log("Waiting for pipe to connect");
      os_yield();
    }
  } while (rc != 0);
  log("Pipe connected", rc);
  DosSleep(200);
  return hPipe;
}

static int close_pipe(HFILE h) {
  return DosDisConnectNPipe(h);
}

int main(int argc, char** argv) {
  auto h = create_pipe("FOO");
  unsigned long num_written;
  log("About to write");
  int rc = DosWrite(h, "Hello\n", 6, &num_written);
  if (rc < 0) {
    log("Error Writing to the pipe");
  }
  log("Wrote [%ld] to pipe", num_written);
  close_pipe(h);

  return 0;
}




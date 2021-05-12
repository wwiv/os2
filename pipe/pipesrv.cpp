#define INCL_DOSDEVIOCTL
#define INCL_DOSERRORS
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

static int saved_key = -1;

static void outch(int ch) {
  fprintf(stdout, "%c", ch);
  if (ch == '\r') {
    fprintf(stdout, "\n");
  }
  fflush(stdout);
}

bool kbhit() {
  if (saved_key == -1) {
    saved_key = _read_kbd(0, 0, 0);
  }
  return saved_key != -1;
}

int getch() {
  if (saved_key != -1) {
    const auto key = saved_key;
    saved_key = -1;
    return key;
  }
  return _read_kbd(0, 1, 0);
}

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
  DosSleep(100);
}

static HFILE create_pipe(const char *name) {
  char pipe_name[200];
  sprintf(pipe_name, "\\PIPE\\WWIV%s", name);
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
  log("Waiting for pipe to connect");
  do {
    rc = DosConnectNPipe(hPipe);
    if (rc != 0) {
      os_yield();
    }
  } while (rc != NO_ERROR);
  log("Pipe connected", rc);
  DosSleep(200);
  return hPipe;
}

static int close_pipe(HFILE h) {
  return DosDisConnectNPipe(h);
}

int main(int argc, char** argv) {
  auto h = create_pipe("1");
  unsigned long num_written;
  log("About to write");
  int rc = DosWrite(h, "Hello\n", 6, &num_written);
  if (rc != NO_ERROR) {
    os_yield();
    log("Error Writing to the pipe");
  }
  log("Wrote [%ld] to pipe", num_written);

  do {
    if (kbhit()) {
      char ch = (char) getch();
      unsigned long num_written;
      int rc = DosWrite(h, &ch, 1, &num_written);
      if (rc != NO_ERROR) {
	os_yield();
	log("Error Writing to the pipe");
      } else if (ch == 27) {
	// ESCAPE to exit
	log("Exiting signaled");
	break;
      }
      outch(ch);
    }
    char ch;
    ULONG num_read;
    int rc = DosRead(h, &ch, 1, &num_read);
    if (rc == NO_ERROR) {
      if (num_read == 0) {
	// If there was no data, rc should be ERROR_NO_DATA (232), so success
	// with 0 read means the pipe is closed.
	fprintf(stderr, "Remote closed pipe.");
	break;
      }
      outch(ch);
      fflush(stdout);
    }
    os_yield();
  } while (true);
  close_pipe(h);

  return 0;
}




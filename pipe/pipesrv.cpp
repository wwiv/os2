#define INCL_DOSDEVIOCTL
#define INCL_DOSERRORS
#define INCL_DOSPROCESS
#include <os2.h>

#include <cctype>
#include <fcntl.h>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#define ENABLE_LOG

static constexpr int PIPE_BUFFER_SIZE = 4000;

static int saved_key = -1;

struct conf_t {
  bool local_echo{false};
  int node_num{1};
  std::string exe;
  int result{0};
};

/*
  Control protocol:

  From  To     MSG   Meaning
 +-----+------+------+--------------------------+
  DOS   OS/2   H      Heartbeat (still alive)
  DOS   OS/2   D      Disconnect (not sure how it'd know)
  OS/2  DOS    D      Disconnect (drop carrier)
  DOS   OS/2   R      FOSSIL Signalled a reboot is needed.
  DOS   OS/2   T      Enable telnet control processing
  DOS   OS/2   B      Switch to binary mode (no telnet processing)
 */

static void outch(int ch) {
  fprintf(stdout, "%c", ch);
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
  DosSleep(1);
}

static HFILE create_pipe(const char *name) {
  char pipe_name[200];
  sprintf(pipe_name, "\\PIPE\\WWIV%s", name);
  log("Dos Create Pipe name : %s", pipe_name);
  HFILE hPipe;
  auto rc = DosCreateNPipe((const unsigned char*) pipe_name,
			   &hPipe,
			   NP_ACCESS_DUPLEX,
			   NP_NOWAIT | NP_TYPE_BYTE | NP_READMODE_BYTE | 0xFF,
			   PIPE_BUFFER_SIZE,
			   PIPE_BUFFER_SIZE,
			   0); // 0 == No Wait.
  log("Dos Create Pipe [name: %s][handle: %d][res: %d]", pipe_name, hPipe, rc);
  return hPipe;
}

static bool connect_pipe(HFILE h) {
  log("Waiting for pipe to connect: [handle: %d]", h);
  int i = 0;
  auto rc = NO_ERROR;
  do {
    if (i++ > 1000) {
      // Failed to connect to pipe.
      return false;
    }
    rc = DosConnectNPipe(h);
    if (rc != 0) {
      // Sleep for 100ms
      DosSleep(100);
      os_yield();
    }
  } while (rc != NO_ERROR);
  log("Pipe connected", rc);
  DosSleep(100);
  return true;
}

static int close_pipe(HFILE h) {
  return DosDisConnectNPipe(h);
}

static bool shutdown_pipes = false;


void _System dos_pipe_loop(unsigned long int raw_conf) {
  conf_t* conf = (conf_t*) raw_conf;
  char pipe_name[81];
  sprintf(pipe_name, "%d", conf->node_num);
  auto h = create_pipe(pipe_name);
  if (!connect_pipe(h)) {
    log("Failed to connect to data pipe.");
    conf->result = 2;
    return;
  }
  strcat(pipe_name, "C");
  auto hc = create_pipe(pipe_name);
  if (!connect_pipe(hc)) {
    log("Failed to connect to control pipe.");
    conf->result = 2;
    return;
  }
  bool stop = false;
  unsigned long num_written;
  do {
    if (kbhit()) {
      auto ch = (char) getch();
      unsigned long num_written;
      auto rc = DosWrite(h, &ch, 1, &num_written);
      if (rc != NO_ERROR) {
	os_yield();
	log("Error Writing to the pipe: %d", rc);
      } else if (ch == 27) {
	// ESCAPE to exit
	log("Exiting signaled. Sending (D)ISCONNECT");
	ch = 'D';
	rc = DosWrite(hc, &ch, 1, &num_written);
	// break;
      }
      if (conf->local_echo) {
	outch(ch);
      }
    }

    char ch;
    ULONG num_read;

    // Read Data
    if (auto rc = DosRead(h, &ch, 1, &num_read); 
	rc == NO_ERROR && num_read > 0) {
      outch(ch);
      continue;
    } else if (rc != 232) {
      // Only yield if we didn't do anything.
      log("DosRead error [%d], numread: [%d]", rc, num_read);
      os_yield();
      break;
    } else {
      os_yield();
    }

    // Read Control
    
    if (auto rc = DosRead(hc, &ch, 1, &num_read); rc == NO_ERROR && num_read > 0) {
      switch (ch) {
      case 'D': 
	// Froced disconnect.
	close_pipe(h);
	close_pipe(hc);
	conf->result = 0;
	return;
      case 'H': {
	// Heartbeat
      } break;
      }
      continue;
    } else if (rc != 232) {
      // Only yield if we didn't do anything.
      log("DosRead error [%d], numread: [%d]", rc, num_read);
      os_yield();
      break;
    } else {
      os_yield();
    }

    auto crc = DosEnterCritSec();
    if (shutdown_pipes) {
      stop = true;
    }
    crc = DosEnterCritSec();

    if (stop) {
      log("Stopping Pipes");
      break;
    }

  } while (true);
  close_pipe(h);
  close_pipe(hc);

}


static void StartFOSSILPipe(conf_t* conf) {
  log("StartFOSSILPipe");
  TID tid = 0;
  DosCreateThread( &tid, dos_pipe_loop, (ULONG) conf, 
		   CREATE_READY | STACK_SPARSE, 8196 );
  DosSleep(100);
}

static void StopFOSSILPipe() {
  log("StopFOSSILPipe");
  auto crc = DosEnterCritSec();
  shutdown_pipes = true;
  crc = DosEnterCritSec();
}

int do_exec(conf_t* conf) {
  return 0;
}


int main(int argc, char** argv) {
  conf_t conf{};
  
  for (int i=0; i <argc; i++) {
    const char* c = argv[i];
    if (*argv[i] == '-' && strlen(c) > 1) {
      switch (std::toupper(*(c+1))) {
	case 'E': conf.local_echo = true; break;
      }
    } else if (strlen(c) > 1 && conf.exe.empty()) {
      conf.exe = c;
    }
  }
  conf.node_num = 1;

  StartFOSSILPipe(&conf);
  // Do Exec.
  do_exec(&conf);
  log("after exec");
  DosSleep(500);
  StopFOSSILPipe();
  return 0;
}

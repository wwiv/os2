// FOSSPIPE - Runs a command under a FOSSIL emulation shell.
#include "fossil.h"
#include "pipe.h"
#include "util.h"
#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <fcntl.h>
#include <io.h>
#include <iostream.h>
#include <process.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

class App {
public:
  App() : comport(0), cmdline(20) {}
  ~App() {}

  int comport;
  Array cmdline;
};

int main(int argc, char** argv) {
  App app;
  cout << app.comport << endl;
  int had_positional = 0;
  for (int i = 1; i < argc; i++) {
    const char* arg = argv[i];
    const int alen = strlen(arg);
    if (!alen) {
      cerr << "WARNING: Empty arg at position: " << i <<  endl;
      continue;
    }
    if (!had_positional && (arg[0] == '-' || arg[0] == '/')) {
      if (alen < 2) {
	cerr << "Invalid argument: " << arg << endl;
	continue;
      }
      // Process switch
      char schar = *(arg+1);
      const char* sval = (arg+2);
      cerr << "Switch: " <<  schar << "; value: '" << sval << "'" << endl;
    } else {
      // Positional arg, must be part of the commandline.
      app.cmdline.push_back(arg);
      cerr << "Positional: '" << arg << "'" << endl;
      had_positional = 1;
    }
  } 

  // Args parsed, do something.

  enable_fossil();
  int ret = _spawnvp(_P_WAIT, app.cmdline.at(0), 
                     (const char**) app.cmdline.items());
  disable_fossil();
  if (ret < 0) {
    cerr << "Error spawning process. " << endl;
    return 1;
  }
  return 0;
}

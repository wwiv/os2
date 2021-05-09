#include "fossil.h"

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

int fossil_enabled = 0;
int carrier = 1;

#define LOBYTE(w) ((unsigned char)(w & 0xff))
#define HIBYTE(w) ((unsigned char)((w >> 8) & 0xff))

#define STATUS_BASE           0x0008
#define STATUS_CARRIER_DETECT 0x0080
#define STATUS_INPUT_AVAIL    0x0100
#define STATUS_INPUT_OVERRUN  0x0200
#define STATUS_OUTPUT_AVAIL   0x2000
#define STATUS_OUTPUT_EMPTY   0x4000
 

void (__interrupt __far *old_int14)();


unsigned status() {
  unsigned r = STATUS_BASE;
  if (carrier) {
    r |= STATUS_CARRIER_DETECT;
  }
  return r;
}

#pragma warning(disable : 4100)
void __interrupt __far int14_handler( unsigned _es, unsigned _ds,
	  unsigned _di, unsigned _si,
	  unsigned _bp, unsigned _sp,
	  unsigned _bx, unsigned _dx,
	  unsigned _cx, unsigned _ax,
	  unsigned _ip, unsigned _cs,
	  unsigned _flags ) { 

  unsigned char func = HIBYTE(_ax);
  log("function: ", func);
  switch (func) {
  case 0x0: {
    // Set baud rate.  Nothing to set since we don't care about BPS
    _ax = status();
  } break;
  case 0x01: {
    // Transmit character with wait
    unsigned char ch = LOBYTE(_ax);
    // TODO add to pipe
    _ax = status();
  } break;
  case 0x02: {
    // Receive characer with wait
    //_ax = read char from pipe
  } break;
  case 0x03: {
    // Request status. 
    // TODO should we mask it?
    _ax = status();
  } break;
  }
}

void enable_fossil() {
  old_int14 = _dos_getvect(0x14);
  _dos_setvect(0x14, (void (__interrupt __far*)()) int14_handler);
  fossil_enabled = 1;
}

void disable_fossil() {
  if (!fossil_enabled) {
    log("ERROR: disable_fossil called when not enabled.");
    return;
  }

  _dos_setvect(0x14, old_int14);
}

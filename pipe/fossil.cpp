#include "fossil.h"

#include "pipe.h"
#include "util.h"
#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <fcntl.h>
#include <io.h>
#include <stdarg.h>
#include <stddef.h>
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

static int num_calls = 0;

#pragma check_stack-
#pragma warning(disable : 4100)

void __interrupt __far int14_handler( unsigned _es, unsigned _ds,
	  unsigned _di, unsigned _si,
	  unsigned _bp, unsigned _sp,
	  unsigned _bx, unsigned _dx,
	  unsigned _cx, unsigned _ax,
	  unsigned _ip, unsigned _cs,
	  unsigned _flags ) { 
  num_calls++;
  
  //log("Interrupt");
  unsigned char func = HIBYTE(_ax);
  //log("function: ", func);
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
  case 0x04: {
    // Request status. 
    // TODO should we mask it?
    _ax = 0x1954;
    _bx = 0x100f;
  } break;
  }
}

void __interrupt __far int14_sig() { 
  char pad[10] = {0};
}

void enable_fossil() {
  old_int14 = _dos_getvect(0x14);

  unsigned char __far * p = (unsigned char __far*) ((void __far*) int14_sig);
  void __far * sig_addr = int14_sig;
  void __far * handler_addr = int14_handler;
  // We offset by 3 since JMP is one, and then two for the address for a near JMP
  int diff = FP_OFF(handler_addr) - FP_OFF(sig_addr) - 3;

  *p = 0xE9;
  *(p+1) = (unsigned char)(diff & 0xff);
  *(p+2) = (unsigned char)(((diff & 0xff00) >> 8) & 0xff);
  *(p+3) = 0x90;
  *(p+4) = 0x90;
  *(p+5) = 0x90;
  // 0x1954 (signature)
  *(p+6) = 0x54;
  *(p+7) = 0x19;
  // Highest supported FOSSIL function.
  *(p+8) = 0x0F;

  log("sig_addr=%p:%p, handler_addr=%p:%p diff=%d/%x", FP_SEG(sig_addr), FP_OFF(sig_addr), 
      FP_SEG(handler_addr), FP_OFF(handler_addr), diff, diff);
  int __far *ip = (int __far*)(p+1); // +1 is where the near address for JMP is
  log("p:%d/%x", *ip, *ip);
  _dos_setvect(0x14, (void (__interrupt __far *)()) int14_sig);
  fossil_enabled = 1;
  log("FOSSIL Enabled");
}

void disable_fossil() {
  if (!fossil_enabled) {
    log("ERROR: disable_fossil called when not enabled.");
    return;
  }

  _dos_setvect(0x14, old_int14);
  log("FOSSIL Disabled: [%d calls]",num_calls);
}


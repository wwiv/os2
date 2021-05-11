#include "fossil.h"

#include "pipe.h"
#include "util.h"
#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <fcntl.h>
#include <io.h>
#include <malloc.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

int fossil_enabled = 0;
int carrier = 1;

#define LOBYTE(w) ((w) & 0xff)
#define HIBYTE(w) (((w) >> 8) & 0xff)

#define STATUS_BASE           0x0008
#define STATUS_CARRIER_DETECT 0x0080
#define STATUS_INPUT_AVAIL    0x0100
#define STATUS_INPUT_OVERRUN  0x0200
#define STATUS_OUTPUT_AVAIL   0x2000
#define STATUS_OUTPUT_EMPTY   0x4000
 
unsigned status() {
  unsigned r = STATUS_BASE;
  if (carrier) {
    r |= STATUS_CARRIER_DETECT;
  }
  // HACK: Claim were were done writing
  r |= STATUS_OUTPUT_AVAIL;
  r |= STATUS_OUTPUT_EMPTY;
  return r;
}


void (__interrupt __far *old_int14)();

int foslog_handle_ = NULL;
int log_count_ = 0;
unsigned orig_psp = 0;
char m[20];

// See https://jeffpar.github.io/kbarchive/kb/075/Q75257/
unsigned int GetPSP() {
  union _REGS r;
  r.h.ah = 0x51;
  _int86(0x21, &r, &r);
  return r.x.bx;
}

void SetPSP(unsigned int psp) {
  union _REGS r;
  r.h.ah = 0x50;
  r.x.bx = psp;
  _int86(0x21, &r, &r);
}

void flog(const char* s) {
  if (foslog_handle_ < 0) {
    log("foslog_handle: %d", foslog_handle_);
    return;
  }

  unsigned int num_written;
  ++log_count_;
  _dos_write(foslog_handle_, s, strlen(s), &num_written);
}


int num_calls = 0;
static int in_int14 = 0;

#pragma check_stack-
#pragma warning(disable : 4100)

void __interrupt __far int14_handler( unsigned _es, unsigned _ds,
	  unsigned _di, unsigned _si,
	  unsigned _bp, unsigned _sp,
	  unsigned _bx, unsigned _dx,
	  unsigned _cx, unsigned _ax,
	  unsigned _ip, unsigned _cs,
	  unsigned _flags ) { 

  if (in_int14) {
    return;
  }
  in_int14 = 1;
  
  int func = (int) HIBYTE(_ax);
#if 0
  cputs("I");
  flog("Interrupt: 0x");
  char m[10];
  flog(itoa(func, m, 16));
  flog("; handle: ");
  flog(itoa(foslog_handle_, m, 10));
  flog("; foscount: ");
  flog(itoa(num_calls, m, 10));
  flog("; logcount: ");
  flog(itoa(log_count_, m, 10));
  flog("\r\n");
#endif  
  num_calls++;
  switch (func) {
  case 0x0: {
    // Set baud rate.  Nothing to set since we don't care about BPS
    _ax = status();
  } break;
  case 0x01:
  case 0x0B: {
    // Transmit character with wait (or no wait for 0b)
    unsigned char ch = (unsigned char) LOBYTE(_ax);
    unsigned int num_written;
    ++log_count_;
    // TODO add to pipe
    _dos_write(foslog_handle_, &ch, 1, &num_written);
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
  case 0x0C: {
    // com peek
  } break;
  case 0x0D: {
    // read keyboard without wait
  } break;
  case 0x0E: {
    // read keyboard with wait
  } break;
  case 0x19: {
    // block write
    /*
|           Parameters:
|               Entry:  CX = Maximum number of characters to transfer
|                       DX = Port number
|                       ES = Segment of user buffer
|                       DI = Offset into ES of user buffer
|               Exit:   AX = Number of characters actually transferred  */
    char __far * buf = (char __far *) MK_FP(_es, _di);
    _dos_write(foslog_handle_, buf, _cx, &_ax);
  } break;
  case 0x1B: {
    // Request status. 
    // TODO should we mask it?
    _ax = 0x1954;
    _bx = 0x100f;
  } break;
  }

  //cputs("]");
  in_int14 = 0;
}

void __interrupt __far int14_sig() { 
  char pad[10] = {0};
}

void enable_fossil() {
  old_int14 = _dos_getvect(0x14);

  _disable();
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
  _dos_setvect(0x14, (void (__interrupt __far *)()) int14_sig);
  _enable();

  log("sig_addr=%p:%p, handler_addr=%p:%p diff=%d/%x", FP_SEG(sig_addr), FP_OFF(sig_addr), 
      FP_SEG(handler_addr), FP_OFF(handler_addr), diff, diff);
  int __far *ip = (int __far*)(p+1); // +1 is where the near address for JMP is
  log("p:%d/%x", *ip, *ip);
  fossil_enabled = 1;
  if (_dos_open("wwivfoss.log", _O_WRONLY, &foslog_handle_) != 0) {
    log("Unable to open FOSSIL log: wwivfoss.log");
  } else {
    lseek(foslog_handle_, 0, SEEK_END);
  }
  log("FOSSIL Enabled. Log File Handle: [%d:%p]", foslog_handle_, foslog_handle_);
  orig_psp = GetPSP();
}

void disable_fossil() {
  if (!fossil_enabled) {
    log("ERROR: disable_fossil called when not enabled.");
    return;
  }

  if (foslog_handle_> 3) {
    _dos_close(foslog_handle_);
    log("Closing wwivfoss.log");
  }

  _disable();
  _dos_setvect(0x14, old_int14);
  _enable();
  log("FOSSIL Disabled: [%d calls], [%d logs]", num_calls, log_count_);
}


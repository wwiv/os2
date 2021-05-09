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


void log(const char* msg, ...) {
#ifdef ENABLE_LOG
  va_list argptr;
  va_start(argptr, msg);
  vfprintf(stderr, msg, argptr);
  va_end(argptr);
  fprintf(stderr, "\r\n");
  fflush(stderr);
#endif
}

void os_yield() {
  // log("os_yield");
  union _REGS r;
  r.x.ax = 0x1680;
  _int86(0x2f, &r, &r);
}

// See http://www.delorie.com/djgpp/doc/rbinter/ix/21/5F.html
/*
INT 21 - Named Pipes - LOCAL DosQNmPHandState

	AX = 5F33h
	BX = handle
Return: CF clear if successful
	    AH = pipe mode bit mask (see #01702)
	    AL = maximum number of instances
	CF set on error
	    AX = error code
Note:	this function was introduced by LAN Manager but is also supported by
	  the Novell DOS Named Pipe Extender, Banyan VINES, OS/2 Virtual DOS
	  Machines, and others
SeeAlso: AX=5F32h,AX=5F34h

Category: network
Flags: partially documented function

INT 21 - Named Pipes - LOCAL DosSetNmPHandState

	AX = 5F34h
	BX = handle
	CX = pipe mode bit mask
	    bit 15: set if nonblocking, clear if blocking
	    bit	 8: set if read in message mode, clear if read in byte mode
Return: CF clear if successful
	CF set if error
	    AX = error code
Note:	this function was introduced by LAN Manager but is also supported by
	  the Novell DOS Named Pipe Extender, Banyan VINES, OS/2 Virtual DOS
	  Machines, and others
SeeAlso: AX=5F32h,AX=5F33h,AX=5F36h


and


INT 21 - Named Pipes - LOCAL DosPeekNmPipe

	AX = 5F35h
	BX = handle
	CX = buffer length
	DS:SI -> buffer
Return: CF set on error
	    AX = error code
	CF clear if successful (LAN Manager v1-v2)
	AX = 0000h if successful (LAN Manager 3.x)
	---if successful---
	    CX = bytes read
	    SI = bytes left in the pipe
	    DX = bytes left in the current message
	    AX = pipe status (v1-v2) (see #01703)
	    DI = pipe status (v3.x)
Note:	this function was introduced by LAN Manager but is also supported by
	  the Novell DOS Named Pipe Extender, Banyan VINES, OS/2 Virtual DOS
	  Machines, and others
SeeAlso: AX=5F38h,AX=5F39h,AX=5F51h

Values for pipe status:
 0001h	disconnected
 0002h	listening
 0003h	connected
 0004h	closing
 */

int DosPeekNmPipe(int handle) {
  union _REGS r;
  union _REGS or;
  struct _SREGS s;
  char buffer[10];
  r.x.ax = 0x5F35;
  r.x.bx = handle;
  s.ds  = _FP_SEG(buffer);
  r.x.si = _FP_OFF(buffer);
  _intdosx(&r, &or, &s);
  if (or.x.ax == 0 || or.x.cflag == 0) {
    if (or.x.ax == 1) {
      // disconnected
      return -1;
    }
    return or.x.cx;
  }
  log("ERROR: DosPeekNmPipe: CF:%d/AX:%d/CX:%d", or.x.cflag, or.x.ax, or.x.cx);
  return 0;
}




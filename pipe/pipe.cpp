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


// See http://www.delorie.com/djgpp/doc/rbinter/ix/21/5F.html
/*
INT 21 - Named Pipes - LOCAL DosQNmPHandState

	AX = 5F33h
	BX = handle
Return: CF clear if successful
a	    AH = pipe mode bit mask (see #01702)
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


void SetPipeNonBlocking(int handle) {
  union _REGS r;
  r.x.ax = 0x5F33;
  r.x.bx = handle;
  _intdos(&r, &r);

  r.h.al = 0;
  r.x.bx = handle;
  r.x.cx = r.x.ax | 0x8000;
  r.x.ax = 0x5F34;

  union _REGS or;
  int result = _intdos(&r, &or);
  if (or.x.cflag) {
    log("Error Setting Pipe Nonnblocking: %d", result);
  }
}


int DosPeekNmPipe(int handle) {
  char buffer[10];
  union _REGS r;
  union _REGS or;
  struct _SREGS s;
  unsigned int c_seg;
  r.x.ax = 0x5F35;
  r.x.bx = handle;
  r.x.cx = 1;
  __asm {
    mov c_seg, ss
      }
  s.ds  = (__segment) &buffer;
  r.x.si = (unsigned int) buffer;
  _intdosx(&r, &or, &s);
  if (or.x.ax == 0 || or.x.cflag == 0) {
    if (or.x.ax == 1 || or.x.ax == 4) {
      // disconnected
      return -1;
    }
    if (or.x.ax != 0x03) {
      //log("WARNING: DosPeekNmPipe: CF:%d/AX:%d/CX:%d", or.x.cflag, or.x.ax, or.x.cx);
    }
    return or.x.cx;
  }
  //log("ERROR: DosPeekNmPipe: CF:%d/AX:%d/CX:%d", or.x.cflag, or.x.ax, or.x.cx);
  return 0;
}


Pipe::Pipe(const char* fn) __far {
  next_char_ = 0;
  int h;
  if (_dos_open(fn, _O_RDWR, &h) != 0) {
    log("ERROR: (Pipe) Unable to open pipe: '%s'", fn);
    handle_ = -1;
  } else {
    handle_ = h;
    num_writes_ = 0;
    num_errors_ = 0;
    bytes_written_ = 0;
    os_yield();
    int peeked = DosPeekNmPipe(handle_);
    log("peeked: [%d]", peeked);
    SetPipeNonBlocking(handle_);
  }
}

void Pipe::close() __far {
  if (handle_ == -1) {
    return;
  }
  _dos_close(handle_);
  handle_ = -1;
}

Pipe::~Pipe() __far {
  if (!is_open()) {
    close();
  }
}

int Pipe::is_open() __far { 
  return handle_ != -1;
}

int Pipe::peek() __far {
  if (next_char_ <= 0) {
    next_char_ = read();
  }
  return next_char_;
}

int Pipe::read() __far {
  int ch = 0;
  if (next_char_ > 0) {
    ch = next_char_;
    next_char_ = 0;
    return ch;
  }

  unsigned num_read;
  int ret = _dos_read(handle_, &ch, 1, &num_read);
  if (ret == 0 && num_read == 0) {
    return 0;
  }
  if (ret == 5) {
    // Not sure why we get permission denied but it fixes itself
    return 0;
  }
  if (ret == 0 && num_read > 0) {
    // Yippie! We got something!
    return ch;
  }
  if (ret != 0) {
    ++num_errors_;
    // DEBUG IT
    write("READ", 4);
    write(ret & 0xff);
    close();
    return -1;
  }
  return ch;
}

int Pipe::blocking_read() {
  for (;;) {
    int r = read();
    if (r) {
      return r;
    }
    os_yield();
  }
}


int Pipe::write(int ch) __far {
  unsigned int num_written;
  ++num_writes_;
  if (_dos_write(handle_, &ch, 1, &num_written) != 0) {
    ++num_errors_;
    close();
    return 0;
  }
  ++bytes_written_;
  return 1;
}

int Pipe::write(const char __far * buf, int maxlen) {
  unsigned int num_written;
  ++num_writes_;
  if (_dos_write(handle_, buf, maxlen, &num_written) != 0) {
    ++num_errors_;
    close();
    return 0;
  }
  bytes_written_ += maxlen;
  return maxlen;
}






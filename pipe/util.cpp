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
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>


Array::Array(int max_items) : max_items_(max_items), last_item_(0) {
  items_ = new char*[max_items];
  for (int i=0; i<max_items; i++) {
    items_[i] = NULL;
  }
}

Array::~Array() {
  for (int i=0; i<last_item_; i++) {
    free(items_[i]);
  }
  delete[] items_;
}

void Array::push_back(const char* item) {
  items_[last_item_++] = strdup(item);
}

int Array::size() const { 
  return last_item_;
}

const char* Array::at(int n) const {
  if (n >= last_item_) {
    fprintf(stderr, "n[%d] > last_item_[%d]", n, last_item_);
    abort();
  }
  return items_[n];
}


// FILE handle for the log file
FILE* log_file = stderr;

void open_log(const char* filename) {
  close_log();
  log_file = fopen(filename, "at");
  if (!log_file) {
    log_file = stderr;
  }
}

void close_log() {
  if (log_file && log_file != stderr) {
    fclose(log_file);
  }
  log_file = stderr;
}

void log(const char* msg, ...) {
#ifndef DISABLE_LOG
  va_list argptr;
  va_start(argptr, msg);
  vfprintf(log_file, msg, argptr);
  va_end(argptr);
  fprintf(log_file, "\r\n");
  fflush(log_file);
#endif
}

void os_yield() {
  // log("os_yield");
  union _REGS r;
  r.x.ax = 0x1680;
  _int86(0x2f, &r, &r);
}

void outch(int ch) {
  fprintf(stdout, "%c", ch);
  if (ch == '\r') {
    fprintf(stdout, "\n");
  }
  fflush(stdout);
}

void sleep(int ms) {
  clock_t then = clock();
  while ((clock() - then) * 1000 / CLOCKS_PER_SEC < ms) {
    // Try to yield timeslices until we're done waiting.
    os_yield();
  }
}



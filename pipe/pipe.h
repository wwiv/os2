#ifndef PIPE_H
#define PIPE_H

#define PIPE_BUFFER_SIZE 4000


int DosPeekNmPipe(int handle);

class __far Pipe {
 public:
  Pipe(const char* fn);
  ~Pipe();
  int read();
  int write(int ch);
  int write(const char __far * buf, int maxlen);
  int peek();

  int is_open();
  void close();
  int handle() { return handle_; }

  int num_writes() { return num_writes_; }
  int num_errors() { return num_errors_; }
  long bytes_written() { return bytes_written_; }

 private:
  int handle_;
  int num_writes_;
  int num_errors_;
  long bytes_written_;
};

#endif // PIPE_H



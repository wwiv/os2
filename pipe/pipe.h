#ifndef PIPE_H
#define PIPE_H

#define PIPE_BUFFER_SIZE 4000


int DosPeekNmPipe(int handle);

class Pipe {
 public:
  Pipe(const char* fn);
  ~Pipe();
  int read();
  int write(int ch);
  int peek();

  int is_open();
  void close();

 private:
  int handle_;
};

#endif // PIPE_H



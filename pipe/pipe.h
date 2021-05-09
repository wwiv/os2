#ifndef INCLUDED_PIPE_H
#define INCLUDED_PIPE_H

#define ENABLE_LOG
#define PIPE_BUFFER_SIZE 4000


void log(const char* msg, ...);
void os_yield();
int DosPeekNmPipe(int handle);

#endif // INCLUDED_PIPE_H

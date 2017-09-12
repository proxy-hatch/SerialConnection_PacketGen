#ifndef MYSOCKET_H_
#define MYSOCKET_H_

#include <unistd.h>
#include <sys/stat.h>

int myOpen(const char *pathname, int flags, mode_t mode);
int myCreat(const char *pathname, mode_t mode);
ssize_t myRead( int fildes, void* buf, size_t nbyte );
ssize_t myWrite( int fildes, const void* buf, size_t nbyte );
int myClose(int fd);

#endif /*MYSOCKET_H_*/

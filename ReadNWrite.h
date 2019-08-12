#ifndef __READ_N_WRITE_H__
#define __READ_N_WRITE_H__
#include "Includes.h"

ssize_t writen(int sockFd, void *block, size_t fixedLength);
ssize_t writePacket(int sockFd, void *packet, size_t length);
ssize_t readn(int sockFd, void *block, size_t fixedLength);
ssize_t readPacket(int sockFd, void *packet, size_t length);

#endif
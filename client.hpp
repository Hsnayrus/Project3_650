#ifndef CLIENT_H
#define CLIENT_H

typedef struct clientInfo {
  int fd;
  int portNum;
  char ipAddress[16];
  int ipSize;
} cInfo_t;

#endif
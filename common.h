#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define TRUE 1
#define FALSE 0

void usage(int, char **);

void logExit(const char *msg);

int addrParse(const char *addrStr, const char *portStr,
              struct sockaddr_storage *storage);

void addrToStr(const struct sockaddr *addr, char *str, size_t strSize);

int serverSockaddrInit(const char *protocol, const char *portStr,
                       struct sockaddr_storage *storage);

void reverse(char *, int);

char *itoa(int, char *, int);

int createServerSocket(char *protocol, char *port);

#endif
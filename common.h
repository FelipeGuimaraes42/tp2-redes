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
#include <time.h>

#define BUFFER_SIZE 1024
#define FALSE 0
#define TRUE 1
#define TYPES_OF_POKEMON 3
#define BOARD_COLUMNS 4
#define BOARD_ROWS 4
#define MILTON "mewtwo"
#define LUGIA "lugia"
#define ZUBAT "zubat"

struct Pokemon
{
    int id;
    char name[10];
    int hits;
    int maxHits;
};

void usage(int, char **);

void logExit(const char *msg);

int addrParse(const char *addrStr, const char *portStr,
              struct sockaddr_storage *storage);

int serverSockaddrInit(const char *protocol, const char *portStr,
                       struct sockaddr_storage *storage);

void my_reverse(char str[], int len);

char *itoa(int num, char *str, int base);

int createServerSocket(char *protocol, char *port);

void generateRandomPokemon(struct Pokemon *pokemon, int id);

#endif
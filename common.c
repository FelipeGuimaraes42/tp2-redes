#include "common.h"

void logExit(const char *msg)
{
  perror(msg);
  exit(EXIT_FAILURE);
}

// Used if user tries to run the program with a different number of params
void usage(int option, char **argv)
{
  // Server usage
  if (option)
  {
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 9000\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  // Client Usage
  printf("usage: %s <server IP> <server port> start\n", argv[0]);
  printf("example: %s 127.0.0.1 9000 start\n", argv[0]);
  exit(EXIT_FAILURE);
}

int addrParse(const char *addrStr, const char *portStr,
              struct sockaddr_storage *storage)
{
  if (addrStr == NULL || portStr == NULL)
  {
    return -1;
  }

  uint16_t port = (uint16_t)atoi(portStr); // unsigned short
  if (port == 0)
  {
    return -1;
  }
  port = htons(port); // host to network short

  struct in_addr inaddr4; // 32-bit IP address
  if (inet_pton(AF_INET, addrStr, &inaddr4))
  {
    struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
    addr4->sin_family = AF_INET;
    addr4->sin_port = port;
    addr4->sin_addr = inaddr4;
    return 0;
  }

  struct in6_addr inaddr6; // 128-bit IPv6 address
  if (inet_pton(AF_INET6, addrStr, &inaddr6))
  {
    struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
    addr6->sin6_family = AF_INET6;
    addr6->sin6_port = port;
    // addr6->sin6_addr = inaddr6
    memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6));
    return 0;
  }

  return -1;
}

int serverSockaddrInit(const char *proto, const char *portStr,
                       struct sockaddr_storage *storage)
{
  uint16_t port = (uint16_t)atoi(portStr); // unsigned short
  if (port == 0)
  {
    return -1;
  }
  port = htons(port); // host to network short

  memset(storage, 0, sizeof(*storage));
  if (0 == strcmp(proto, "v4"))
  {
    struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
    addr4->sin_family = AF_INET;
    addr4->sin_addr.s_addr = INADDR_ANY;
    addr4->sin_port = port;
    return 0;
  }
  else if (0 == strcmp(proto, "v6"))
  {
    struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
    addr6->sin6_family = AF_INET6;
    addr6->sin6_addr = in6addr_any;
    addr6->sin6_port = port;
    return 0;
  }
  else
  {
    return -1;
  }
}

/*
 * function to reverse a string
  reference: https://www.techcrashcourse.com/2016/02/c-program-to-implement-your-own-itoa-function.html
 */
void my_reverse(char *str, int len)
{
  int start, end;
  char temp;
  for (start = 0, end = len - 1; start < end; start++, end--)
  {
    temp = *(str + start);
    *(str + start) = *(str + end);
    *(str + end) = temp;
  }
}

/*
 * function to convert an integer into a string
  reference: https://www.techcrashcourse.com/2016/02/c-program-to-implement-your-own-itoa-function.html
 */
char *itoa(int num, char *str, int base)
{
  int i = 0;
  int isNegative = FALSE;

  /* A zero is same "0" string in all base */
  if (num == 0)
  {
    str[i] = '0';
    str[i + 1] = '\0';
    return str;
  }

  /* negative numbers are only handled if base is 10
     otherwise considered unsigned number */
  if (num < 0 && base == 10)
  {
    isNegative = TRUE;
    num = -num;
  }

  while (num != 0)
  {
    int rem = num % base;
    str[i++] = (rem > 9) ? (rem - 10) + 'A' : rem + '0';
    num = num / base;
  }

  /* Append negative sign for negative numbers */
  if (isNegative)
  {
    str[i++] = '-';
  }

  str[i] = '\0';

  my_reverse(str, i);

  return str;
}

int createServerSocket(char *protocol, char *port)
{

  struct sockaddr_storage storage;
  if (serverSockaddrInit(protocol, port, &storage) != 0)
  {
    logExit("wrong params");
  }

  int sockfd = socket(storage.ss_family, SOCK_DGRAM, 0);
  if (sockfd == -1)
  {
    logExit("socket");
  }

  int enable = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) != 0)
  {
    logExit("setsockopt");
  }

  struct sockaddr *serverAddr = (struct sockaddr *)(&storage);
  if (bind(sockfd, serverAddr, sizeof(storage)) != 0)
  {
    logExit("bind");
  }
  return sockfd;
}

void generateRandomPokemon(struct Pokemon *pokemon, int id)
{
  int poke = rand() % TYPES_OF_POKEMON;
  if (poke == 0)
  {
    pokemon->hits = 0;
    pokemon->maxHits = 1;
    char nome[7];
    strcpy(nome, ZUBAT);
    strcpy(pokemon->name, nome);
    //pokemon->name = nome;
    pokemon->id = id;
  }
  else if (poke == 1)
  {
    pokemon->hits = 0;
    pokemon->maxHits = 2;
    char nome[7];
    strcpy(nome, LUGIA);
    strcpy(pokemon->name, nome);
    //pokemon->name = nome;
    pokemon->id = id;
  }
  else if (poke == 2)
  {
    pokemon->hits = 0;
    pokemon->maxHits = 3;
    char nome[7];
    strcpy(nome, MILTON);
    strcpy(pokemon->name, nome);
    //pokemon->name = nome;
    pokemon->id = id;
  }
  else
  {
    printf("Dunno how you reached here pal\n");
    exit(EXIT_FAILURE);
  }
}
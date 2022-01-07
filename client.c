#include "common.h"

int main(int argc, char **argv)
{
  if (argc != 4)
  {
    usage(FALSE, argv);
  }

  if (strcmp(argv[3], "start") != 0)
  {
    logExit("wrong initialization keyword");
  }

  struct sockaddr_storage storage;
  if (addrParse(argv[1], argv[2], &storage) != 0)
  {
    usage(FALSE, argv);
  }

  int clientSock = socket(storage.ss_family, SOCK_DGRAM, 0);
  if (clientSock == -1)
  {
    logExit("socket");
  }

  struct sockaddr *addr = (struct sockaddr *)(&storage);

  char addrStr[BUFFER_SIZE];
  addrToStr(addr, addrStr, BUFFER_SIZE);
  printf("%s\n", addrStr);

  char buffer[BUFFER_SIZE];
  memset(buffer, 0, BUFFER_SIZE);

  strcpy(buffer, argv[3]);
  sendto(clientSock, buffer, strlen(buffer), 0, addr, sizeof(struct sockaddr_storage));
  printf("[+]Data Send: %s\n", buffer);

  socklen_t addrSize = sizeof(struct sockaddr);
  socklen_t storageSize = sizeof(struct sockaddr_storage);

  while (1)
  {
    memset(buffer, 0, BUFFER_SIZE);
    printf("> ");
    fgets(buffer, BUFFER_SIZE - 1, stdin);

    int count = sendto(clientSock, buffer, strlen(buffer), 0, addr, storageSize);

    if (count != strlen(buffer))
    {
      logExit("send");
    }
    if (strcasecmp(buffer, "kill\n") == 0)
    {
      break;
    }
    
    memset(buffer, 0, BUFFER_SIZE);

    count = recvfrom(clientSock, buffer, BUFFER_SIZE, 0, addr, &addrSize);
    if (count == 0)
    {
      break;
    }
    printf("Data received: %s", buffer);
  }

  close(clientSock);

  return 0;
}

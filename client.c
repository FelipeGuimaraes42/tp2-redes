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
  // printf("%s\n", addrStr);

  char buffer[BUFFER_SIZE];
  memset(buffer, 0, BUFFER_SIZE);

  strcpy(buffer, argv[3]);
  sendto(clientSock, buffer, strlen(buffer), 0, addr, sizeof(struct sockaddr_storage));

  socklen_t addrSize = sizeof(struct sockaddr);
  socklen_t storageSize = sizeof(struct sockaddr_storage);

  int response = 1;
  while (response < 5)
  {
    memset(buffer, 0, BUFFER_SIZE);

    int count = recvfrom(clientSock, buffer, BUFFER_SIZE, 0, addr, &addrSize);
    if (count == 0)
    {
      break;
    }
    printf("%s %d\n", buffer, response);
    response++;
  }

  int turn = 0;

  while (1)
  {
    memset(buffer, 0, BUFFER_SIZE);
    printf("> ");
    fgets(buffer, BUFFER_SIZE - 1, stdin);
    printf("< ");

    int count = sendto(clientSock, buffer, strlen(buffer), 0, addr, storageSize);
    if (count != strlen(buffer))
    {
      sendto(clientSock, "quit\n", strlen("quit\n"), 0, addr, storageSize);
      logExit("send");
    }

    if (strcasecmp(buffer, "quit\n") == 0)
    {
      break;
    }
    else if (strcasecmp(buffer, "getdefenders\n") == 0)
    {
      int count = recvfrom(clientSock, buffer, BUFFER_SIZE, 0, addr, &addrSize);
      if (count == 0)
      {
        break;
      }
      printf("%s\n", buffer);
    }
    else if (strcasecmp(strtok(buffer, " "), "getturn") == 0)
    {
      char strTurn[2];
      if (strcmp(strtok(NULL, " \n"), itoa(turn, strTurn, 10)) != 0)
      {
        sendto(clientSock, "quit\n", strlen("quit\n"), 0, addr, storageSize);
        logExit("wrong turn");
      }
      // one for each base
      for (int i = 0; i < 4; i++)
      {
        int count = recvfrom(clientSock, buffer, BUFFER_SIZE, 0, addr, &addrSize);
        if (count == 0)
        {
          break;
        }
        printf("base %d\n", i + 1);
        printf("%s", buffer);
      }
      turn++;
    }
    else if (strcasecmp(strtok(buffer, " "), "shot") == 0)
    {
      int count = recvfrom(clientSock, buffer, BUFFER_SIZE, 0, addr, &addrSize);
      if (count == 0)
      {
        break;
      }
      printf("shotresp %s\n", buffer);
    }
    else
    {
      int count = recvfrom(clientSock, buffer, BUFFER_SIZE, 0, addr, &addrSize);
      if (count == 0)
      {
        break;
      }
      printf("gameover %s\n", buffer);
      sendto(clientSock, "quit\n", strlen("quit\n"), 0, addr, storageSize);
      break;
    }
  }

  close(clientSock);

  return 0;
}

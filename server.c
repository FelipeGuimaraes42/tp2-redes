#include "common.h"

int main(int argc, char **argv)
{
  if (argc != 3)
  {
    usage(TRUE, argv);
  }

  char *protocol = argv[1];
  int port = atoi(argv[2]);

  int ports[4] = {0};

  for (int i = 0; i < 4; i++)
  {
    ports[i] = port + i;
    //printf("%d  ", ports[i]);
  }
  //printf("\n");

  int sockets[4];

  for (int i = 0; i < 4; i++)
  {
    char *strPort = itoa(ports[i], strPort, 10);
    //printf("%s  ", strPort);
    sockets[i] = createServerSocket(protocol, strPort);
  }
  //printf("\n");

  struct sockaddr_storage clientStorage;
  struct sockaddr *clientAddr;
  clientAddr = (struct sockaddr *)&clientStorage;
  socklen_t clientAddrSize = sizeof(struct sockaddr);

  char buffer[BUFFER_SIZE];

  // for(int i = 0; i < 4; i++) {
  //   recvfrom(sockets[i], buffer, BUFFER_SIZE, 0, clientAddr, &clientAddrSize);
  //   printf("[+]Data Received: %s, into server %d", buffer, i);
  // }

  memset(clientAddr, 0, sizeof(clientStorage));

  int count = recvfrom(sockets[0], buffer, BUFFER_SIZE, 0, clientAddr, &clientAddrSize);
  if (count == 0)
  {
    logExit("empty message");
  }
  printf("[+]Data Received: %s\n", buffer);

  while (1)
  {
    memset(clientAddr, 0, sizeof(clientStorage));

    int count = recvfrom(sockets[0], buffer, BUFFER_SIZE, 0, clientAddr, &clientAddrSize);
    if (count == 0)
    {
      logExit("empty message");
    }
    printf("[+]Data Received: %s", buffer);

    if (strcasecmp(buffer, "kill\n") == 0)
    {
      break;
    }

    memset(buffer, 0, BUFFER_SIZE);
  }

  for (int i = 0; i < 4; i++)
  {
    close(sockets[i]);
  }

  return 0;
}

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

  // // 0 is the borning position
  // // 5 is the pokedex location
  // struct Pokemon gameRow[4][5] = {0};
  // // struct Pokemon *gameRow1[5];
  // // struct Pokemon *gameRow2[5];
  // // struct Pokemon *gameRow3[5];

  // srand(time(NULL));

  // struct Pokemon **pokemons = {0};
  // int numberOfPokemon = 0;

  // char oneToFour[4] = {'1', '2', '3', '4'};

  for (int i = 0; i < 4; i++)
  {
    ports[i] = port + i;
    // printf("%d  ", ports[i]);
  }
  // printf("\n");

  int sockets[4];
  // struct sockaddr *serverAddr[4];

  for (int i = 0; i < 4; i++)
  {
    char *strPort = itoa(ports[i], strPort, 10);
    // printf("%s  ", strPort);
    sockets[i] = createServerSocket(protocol, strPort);
  }
  // printf("\n");

  struct sockaddr_storage clientStorage;
  struct sockaddr *clientAddr;
  clientAddr = (struct sockaddr *)&clientStorage;
  socklen_t addrSize = sizeof(struct sockaddr);
  socklen_t storageSize = sizeof(struct sockaddr_storage);

  char buffer[BUFFER_SIZE];

  memset(clientAddr, 0, sizeof(clientStorage));

  int count = recvfrom(sockets[0], buffer, BUFFER_SIZE, 0, clientAddr, &addrSize);
  if (count == 0)
  {
    logExit("empty message");
  }
  printf("[+]Data Received: %s by server %d\n", buffer, ports[0]);

  memset(buffer, 0, BUFFER_SIZE);

  for (int i = 0; i < 4; i++)
  {
    strcpy(buffer, "game started: path");
    count = sendto(sockets[i], buffer, sizeof(buffer), 0, clientAddr, storageSize);
    // if (count != strlen(buffer))
    // {
    //   logExit("send");
    // }
    // printf("[+]Data Sent: %s by server %d\n", buffer, ports[i]);
  }

  fflush(stdin);

  while (1)
  {
    memset(clientAddr, 0, sizeof(clientStorage));
    memset(buffer, 0, BUFFER_SIZE);

    int count = recvfrom(sockets[3], buffer, BUFFER_SIZE, 0, clientAddr, &addrSize);
    if (count == 0)
    {
      logExit("empty message");
    }
    printf("[+]Data Received: %s", buffer);

    if (strcasecmp(buffer, "quit\n") == 0)
    {
      break;
    }

    if (strcasecmp(buffer, "getdefenders\n") == 0)
    {
      memset(buffer, 0, BUFFER_SIZE);
      strcpy(buffer, "defender [[1, 0], [3, 0], [4, 1], [2, 2], [3, 3], [4, 4]]");
      printf("%s\n", buffer);
      sendto(sockets[3], buffer, sizeof(buffer), 0, clientAddr, storageSize);
    }


    // if (strcasecmp(strtok(buffer, " "), "getturn") == 0)
    // {
    //   // first wave
    //   if (strcmp(strtok(NULL, "\n"), "0") == 0)
    //   {
    //     for (int i = 0; i < 4; i++)
    //     {
    //       pokemons[numberOfPokemon] = generateRandomPokemon(numberOfPokemon);
    //       gameRow[i][0] = *pokemons[numberOfPokemon];
    //       numberOfPokemon++;
    //       sendto(sockets[3], pokemons[i]->name, sizeof(buffer), 0, clientAddr, storageSize);
    //     }
    //   }
    //   else
    //   {
    //     // follow the baile
    //     for (int i = 0; i < 4; i++)
    //     {
    //       pokemons[numberOfPokemon] = generateRandomPokemon(numberOfPokemon);
    //       gameRow[i][0] = *pokemons[numberOfPokemon];
    //       numberOfPokemon++;
    //       sendto(sockets[3], pokemons[i]->name, sizeof(buffer), 0, clientAddr, storageSize);
    //     }
    //   }

    //   memset(buffer, 0, BUFFER_SIZE);
    //   strcpy(buffer, "defender [[1, 0], [3, 0], [4, 1], [2, 2], [3, 3], [4, 4]]");
    //   printf("%s\n", buffer);
    //   sendto(sockets[3], buffer, sizeof(buffer), 0, clientAddr, storageSize);
    // }
  }

  for (int i = 0; i < 4; i++)
  {
    close(sockets[i]);
  }

  return 0;
}

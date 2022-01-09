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

  /*
    the game board will be a matrix and
    it will store the pokemon ids
  */
  int attackersBoard[BOARD_ROWS][BOARD_COLUMNS];
  memset(attackersBoard, -1, sizeof(attackersBoard[0][0]) * BOARD_ROWS * BOARD_COLUMNS);

  int defendersBoard[BOARD_ROWS][BOARD_COLUMNS];
  memset(defendersBoard, -1, sizeof(defendersBoard[0][0]) * BOARD_ROWS * BOARD_COLUMNS);

  // char oneToFour[4] = {'1', '2', '3', '4'};

  // for (int i = 0; i < BOARD_ROWS; i++)
  // {
  //   for (int j = 0; j < BOARD_COLUMNS; j++)
  //   {
  //     printf("%d  ", attackersBoard[i][j]);
  //   }
  //   printf("\n");
  // }

  srand(time(NULL));

  struct Pokemon pokemons[300];
  int numberOfPokemon = 0;

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
    char strPort[10];
    sockets[i] = createServerSocket(protocol, itoa(ports[i], strPort, 10));
    // printf("%s  ", strPort);
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
    char receivedMessage[BUFFER_SIZE];
    memset(clientAddr, 0, sizeof(clientStorage));
    memset(buffer, 0, BUFFER_SIZE);
    memset(receivedMessage, 0, BUFFER_SIZE);

    int count = recvfrom(sockets[3], buffer, BUFFER_SIZE, 0, clientAddr, &addrSize);
    if (count == 0)
    {
      logExit("empty message");
    }
    printf("[+]Data Received: %s", buffer);
    strcpy(receivedMessage, buffer);

    if (strcasecmp(buffer, "quit\n") == 0)
    {
      break;
    }

    if (strcasecmp(buffer, "getdefenders\n") == 0)
    {
      memset(buffer, 0, BUFFER_SIZE);
      strcpy(buffer, "defender [[1, 0], [3, 0], [4, 1], [2, 2], [3, 3], [4, 4]]");
      sendto(sockets[3], buffer, sizeof(buffer), 0, clientAddr, storageSize);
    }

    if (strcasecmp(strtok(buffer, " "), "getturn") == 0)
    {
      // first wave
      char turn[3];
      strcpy(turn, strtok(NULL, " \n"));
      if (strcmp(turn, "0") == 0)
      {
        for (int i = 0; i < 4; i++)
        {
          generateRandomPokemon(&(pokemons[numberOfPokemon]), numberOfPokemon);
          attackersBoard[i][0] = numberOfPokemon;

          char message[BUFFER_SIZE];
          strcpy(message, "fixed location 1\n");
          char pokeId[4];
          char pokeHits[2];
          char pokeName[10];
          strcpy(pokeName, pokemons[i].name);
          strcpy(pokeId, itoa(pokemons[i].id, pokeId, 10));
          strcpy(pokeHits, itoa(pokemons[i].hits, pokeHits, 10));

          strcat(message, pokeId);
          strcat(message, " ");
          strcat(message, pokeName);
          strcat(message, " ");
          strcat(message, pokeHits);
          strcat(message, "\n");
          numberOfPokemon++;

          strcat(message, "\nturn 0\nfixed location 2\n");
          strcat(message, "\nturn 0\nfixed location 3\n");

          sendto(sockets[3], message, sizeof(message), 0, clientAddr, storageSize);
        }
      }
      else
      {
        for (int i = 0; i < 4; i++)
        {
          sendto(sockets[3], "TBD\n", sizeof("TBD\n"), 0, clientAddr, storageSize);
        }
      }
    }

    if (strcasecmp(strtok(buffer, " "), "shot") == 0)
    {
      printf("%d\n", pokemons[1].maxHits);
      char *row = strtok(receivedMessage, " ");
      row = strtok(NULL, " ");
      char *column = strtok(NULL, " ");
      char *id = strtok(NULL, " \n");

      memset(buffer, 0, BUFFER_SIZE);
      strcpy(buffer, row);
      strcat(buffer, " ");
      strcat(buffer, column);
      strcat(buffer, " ");
      strcat(buffer, id);
      strcat(buffer, "\n");

      int intId = atoi(id);
      char nome[10];
      strcpy(nome, pokemons[1].name);
      printf("nome %s\n", nome);

      sendto(sockets[3], buffer, sizeof(buffer), 0, clientAddr, storageSize);
    }
  }

  for (int i = 0; i < 4; i++)
  {
    close(sockets[i]);
  }

  return 0;
}

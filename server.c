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

  int pokemonHitted = 0;
  int pokemonWhoReachedPokedex = 0;

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
      // Pokemon fora da borda atacam em suas posições e num posição acima
      // Pokémon na linha 0 ou 3 atacam somente suas linhas
      strcpy(buffer, "defender [[0, 0], [0, 2], [1, 3], [2, 1], [3, 2], [3, 3]]");
      for (int i = 0; i < BOARD_ROWS; i++)
      {
        for (int j = 0; j < BOARD_COLUMNS; j++)
        {
          printf("%d  ", defendersBoard[i][j]);
        }
        printf("\n");
      }
      sendto(sockets[3], buffer, sizeof(buffer), 0, clientAddr, storageSize);
    }

    if (strcasecmp(strtok(buffer, " "), "getturn") == 0)
    {
      // first wave
      char turn[3];
      strcpy(turn, strtok(NULL, " \n"));

      defendersBoard[0][0] = defendersBoard[0][2] = defendersBoard[1][3] = defendersBoard[2][1] =
          defendersBoard[3][2] = defendersBoard[3][3] = 1;
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
      char *row = strtok(receivedMessage, " ");
      row = strtok(NULL, " ");
      char *column = strtok(NULL, " ");
      char *id = strtok(NULL, " \n");
      char status[2];
      strcpy(status, "1");

      int intRow = atoi(row);
      int intColumn = atoi(column);
      int intId = atoi(id);

      // printf("%d\n", defendersBoard[intRow][intColumn]);

      if ((defendersBoard[intRow][intColumn] == 1) && (intId >= 0))
      {
        //Only one attack per turn
        defendersBoard[intRow][intColumn] = -1;

        int pokeColumn, pokeRow;
        pokeColumn = pokeRow = -1;
        for (int i = 0; i < BOARD_ROWS; i++)
        {
          for (int j = 0; j < BOARD_COLUMNS; j++)
          {
            if (attackersBoard[i][j] == intId)
            {
              pokeRow = i;
              pokeColumn = j;
              break;
            }
          }
        }
        // Didn't find a Pokémon with that ID
        if (pokeColumn == -1 || pokeRow == -1)
        {
          printf("There's no attacker Pokemon there!\n");
          strcpy(status, "1");
        }
        else
        {
          // Verificar se o Pokémon de defesa consegue acertar esse cara
          if (intRow == 0 || intRow == 3)
          {
            // So consegue atacar onde ele está
            if (intRow == pokeRow && intColumn == pokeColumn)
            {
              pokemons[intId].hits++;
              printf("%s was hitted! It has %d hits left\n", pokemons[intId].name, pokemons[intId].maxHits - pokemons[intId].hits);
              if (pokemons->maxHits - pokemons->hits == 0)
              {
                printf("%s is out of combat!\n", pokemons[intId].name);
                attackersBoard[pokeRow][pokeColumn] = -1;
              }
              pokemonHitted++;
              strcpy(status, "0");
            }
            else
            {
              strcpy(status, "1");
            }
          }
          else
          {
            // Consegue atacar onde ele esta e uma linha acima
            if ((intRow == pokeRow || intRow == (pokeRow + 1)) && intColumn == pokeColumn)
            {
              pokemons[intId].hits++;
              printf("%s was hitted! It has %d hits left\n", pokemons[intId].name, pokemons[intId].maxHits - pokemons[intId].hits);
              if (pokemons->maxHits - pokemons->hits == 0)
              {
                printf("%s is out of combat!\n", pokemons[intId].name);
                attackersBoard[pokeRow][pokeColumn] = -1;
              }
              pokemonHitted++;
              strcpy(status, "0");
            }
            else
            {
              strcpy(status, "1");
            }
          }
        }
        memset(buffer, 0, BUFFER_SIZE);
        strcpy(buffer, row);
        strcat(buffer, " ");
        strcat(buffer, column);
        strcat(buffer, " ");
        strcat(buffer, id);
        strcat(buffer, " ");
        strcat(buffer, status);
        strcat(buffer, "\n");

        sendto(sockets[3], buffer, sizeof(buffer), 0, clientAddr, storageSize);
      }
      else
      {
        printf("There's no defender Pokemon there or the pokemon needs to recharge!\n");
        memset(buffer, 0, BUFFER_SIZE);
        strcpy(buffer, row);
        strcpy(status, "1");
        strcat(buffer, " ");
        strcat(buffer, column);
        strcat(buffer, " ");
        strcat(buffer, id);
        strcat(buffer, " ");
        strcat(buffer, status);
        strcat(buffer, "\n");

        sendto(sockets[3], buffer, sizeof(buffer), 0, clientAddr, storageSize);
      }
    }
  }

  for (int i = 0; i < 4; i++)
  {
    close(sockets[i]);
  }

  return 0;
}

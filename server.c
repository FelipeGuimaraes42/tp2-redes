#include "common.h"

int main(int argc, char **argv)
{
  if (argc != 3)
  {
    usage(TRUE, argv);
  }

  char *protocol = argv[1];
  int firstPort = atoi(argv[2]);

  // for a real random
  srand(time(NULL));

  int ports[NUMBER_OF_SERVERS] = {0};
  int sockets[NUMBER_OF_SERVERS];

  // The ports need to be sequential
  for (int i = 0; i < NUMBER_OF_SERVERS; i++)
  {
    ports[i] = firstPort + i;
  }

  for (int i = 0; i < NUMBER_OF_SERVERS; i++)
  {
    char strPort[10];
    sockets[i] = createServerSocket(protocol, itoa(ports[i], strPort, 10));
  }

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

  for (int i = 0; i < NUMBER_OF_SERVERS; i++)
  {
    strcpy(buffer, "game started: path");
    count = sendto(sockets[i], buffer, sizeof(buffer), 0, clientAddr, storageSize);
  }

  fflush(stdin);

  /*
    the game board will be a matrix and
    it will store the pokemon ids for the attackers
    and it will store a positive number for defenders
  */
  int attackersBoard[BOARD_ROWS][BOARD_COLUMNS];
  memset(attackersBoard, -1, sizeof(attackersBoard[0][0]) * BOARD_ROWS * BOARD_COLUMNS);

  int defendersBoard[BOARD_ROWS][BOARD_COLUMNS];
  memset(defendersBoard, -1, sizeof(defendersBoard[0][0]) * BOARD_ROWS * BOARD_COLUMNS);

  // As I don't like pointers, I setted the max number of Poké to NUMBER_MAX_POKEMON
  struct Pokemon pokemons[NUMBER_MAX_POKEMON];
  int numberOfPokemon = 0;

  int pokemonHitted = 0;
  int pokemonWhoReachedPokedex = 0;
  int intTurn = 0;

  clock_t timer = clock();

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

    // Makes a copy of the buffer to future manipulations
    strcpy(receivedMessage, buffer);

    if (strcasecmp(buffer, "quit\n") == 0)
    {
      break;
    }

    else if (strcasecmp(buffer, "getdefenders\n") == 0)
    {
      memset(buffer, 0, BUFFER_SIZE);
      // Pokemon at 1st or 4th only can attack where they are
      // Pokémon at 2nd of 3rd can attack where they are and a row above
      strcpy(buffer, "defender [[0, 0], [0, 2], [1, 3], [2, 1], [3, 2], [3, 3]]");
      sendto(sockets[3], buffer, sizeof(buffer), 0, clientAddr, storageSize);
    }

    else if (strcasecmp(strtok(buffer, " "), "getturn") == 0)
    {
      char turn[3];
      strcpy(turn, strtok(NULL, " \n"));

      defendersBoard[0][0] = defendersBoard[0][2] = defendersBoard[1][3] = defendersBoard[2][1] =
          defendersBoard[3][2] = defendersBoard[3][3] = 1;
      // first turn
      if (strcmp(turn, "0") == 0)
      {
        for (int i = 0; i < BOARD_ROWS; i++)
        {
          generateRandomPokemon(&(pokemons[numberOfPokemon]), numberOfPokemon);
          attackersBoard[i][0] = numberOfPokemon;

          char message[BUFFER_SIZE];
          strcpy(message, "turn ");
          strcat(message, turn);
          strcat(message, "\nfixed location 1\n");

          char pokeName[10];
          strcpy(pokeName, pokemons[i].name);
          char pokeId[4];
          strcpy(pokeId, itoa(pokemons[i].id, pokeId, 10));
          char pokeHits[2];
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
          strcat(message, "\nturn 0\nfixed location 4\n\n");

          sendto(sockets[3], message, sizeof(message), 0, clientAddr, storageSize);
        }
      }
      // other turns
      else
      {
        // Aqui o que eu tenho que fazer é o seguinte: andar com os pokemon vivos uma casa
        //  para a direita e spawnar novos monstros nas primeiras casas
        for (int i = BOARD_ROWS - 1; i >= 0; i--)
        {
          for (int j = BOARD_COLUMNS - 1; j >= 0; j--)
          {
            if (attackersBoard[i][j] >= 0)
            {
              if (j == BOARD_COLUMNS - 1)
              {
                pokemonWhoReachedPokedex++;
                attackersBoard[i][j] = -1;
              }
              else
              {
                attackersBoard[i][j + 1] = attackersBoard[i][j];
                attackersBoard[i][j] = -1;
              }
            }
          }
        }
        for (int i = 0; i < BOARD_ROWS; i++)
        {
          generateRandomPokemon(&(pokemons[numberOfPokemon]), numberOfPokemon);
          attackersBoard[i][0] = numberOfPokemon;
          char message[BUFFER_SIZE];
          memset(message, 0, BUFFER_SIZE);
          int index = intTurn * BOARD_ROWS + i;
          for (int j = 0; j < BOARD_COLUMNS; j++)
          {

            char fixedLocation[2];
            strcat(message, "turn ");
            strcat(message, turn);
            strcat(message, "\nfixed location ");
            strcat(message, itoa(j + 1, fixedLocation, 10));
            strcat(message, "\n");
            if (index >= 0)
            {
              if ((pokemons[index].maxHits - pokemons[index].hits) > 0)
              {
                char pokeName[10];
                strcpy(pokeName, pokemons[index].name);
                char pokeId[4];
                strcpy(pokeId, itoa(pokemons[index].id, pokeId, 10));
                char pokeHits[2];
                strcpy(pokeHits, itoa(pokemons[index].hits, pokeHits, 10));

                strcat(message, pokeId);
                strcat(message, " ");
                strcat(message, pokeName);
                strcat(message, " ");
                strcat(message, pokeHits);
                strcat(message, "\n\n");
              }
              else
              {
                strcat(message, "\n");
              }
            }
            else
            {
              strcat(message, "\n");
            }
            index -= BOARD_ROWS;
          }
          numberOfPokemon++;
          sendto(sockets[3], message, sizeof(message), 0, clientAddr, storageSize);
        }
      }
      intTurn++;
    }

    else if (strcasecmp(strtok(buffer, " "), "shot") == 0)
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

      if ((defendersBoard[intRow][intColumn] == 1) && (intId >= 0))
      {
        // Only one attack per turn
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
          if (intRow == 0 || intRow == BOARD_ROWS - 1)
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
    else
    {
      memset(buffer, 0, BUFFER_SIZE);

      timer = clock() - timer;
      // int timeTaken = (int)(((double)timer)/CLOCKS_PER_SEC);
      double doubleTime = ((double)timer) / CLOCKS_PER_SEC;
      //printf("Timer: %f", doubleTime);

      int timeTaken = (int)doubleTime;

      char finalHits[5];
      strcpy(buffer, itoa(pokemonHitted, finalHits, 10));

      strcat(buffer, " ");
      char notDestroyedPokemon[4];
      strcat(buffer, itoa(pokemonWhoReachedPokedex, notDestroyedPokemon, 10));

      strcat(buffer, " ");
      char secondsToFinish[10];
      strcat(buffer, itoa(timeTaken, secondsToFinish, 10));

      sendto(sockets[3], buffer, sizeof(buffer), 0, clientAddr, storageSize);
    }
  }

  for (int i = 0; i < NUMBER_OF_SERVERS; i++)
  {
    close(sockets[i]);
  }

  return 0;
}

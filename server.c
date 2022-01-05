#include "common.h"

int main(int argc, char **argv){
  if(argc != 3){
    usage(TRUE, argv);
  }

  struct sockaddr_storage storage;
  if (serverSockaddrInit(argv[1], argv[2], &storage) != 0) {
    usage(TRUE, argv);
  }

  char buffer[BUFFER_SIZE];

  int sockfd;
  sockfd = socket(storage.ss_family, SOCK_DGRAM, 0);
  if (sockfd == -1) {
      logExit("socket");
  }

  int enable = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) != 0) {
      logExit("setsockopt");
  }

  struct sockaddr *addr = (struct sockaddr *)(&storage);
  if (0 != bind(sockfd, addr, sizeof(storage))) {
      logExit("bind");
  }

  struct sockaddr *clientAddr;
  memset(&clientAddr, 0, sizeof(clientAddr));

  socklen_t addr_size = sizeof(clientAddr);
  recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)& clientAddr, &addr_size);
  printf("[+]Data Received: %s", buffer);

  return 0;

}

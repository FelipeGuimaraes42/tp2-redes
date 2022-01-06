#include "common.h"

int main(int argc, char **argv){
  if(argc != 3){
    usage(TRUE, argv);
  }

  struct sockaddr_storage storage;
  if (serverSockaddrInit(argv[1], argv[2], &storage) != 0) {
    usage(TRUE, argv);
  }

  int sockfd = socket(storage.ss_family, SOCK_DGRAM, 0);
  if (sockfd == -1) {
      logExit("socket");
  }

  int enable = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) != 0) {
      logExit("setsockopt");
  }

  struct sockaddr *serverAddr = (struct sockaddr *)(&storage);
  if (bind(sockfd, serverAddr, sizeof(storage)) != 0) {
      logExit("bind");
  }

  char addrStr[BUFFER_SIZE];
  addrToStr(serverAddr, addrStr, BUFFER_SIZE);
  printf("%s\n", addrStr);

  struct sockaddr_storage clientStorage;
	struct sockaddr *clientAddr;
	clientAddr = (struct sockaddr *) &clientStorage;
	socklen_t clientAddrSize = sizeof(struct sockaddr);

  char buffer[BUFFER_SIZE];

  recvfrom(sockfd, buffer, BUFFER_SIZE, 0, clientAddr, &clientAddrSize);
  printf("[+]Data Received: %s", buffer);

  close(sockfd);

  return 0;
}

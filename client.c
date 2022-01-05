#include "common.h"

int main(int argc, char **argv){
  if(argc != 4) {
    usage(FALSE, argv);
  }

  struct sockaddr_storage storage;
  if (addrParse(argv[1], argv[2], &storage) != 0) {
      usage(FALSE, argv);
  }

  if(strcmp(argv[3], "start") != 0){
    logExit("wrong initialization keyword.");
  }

  char buffer[BUFFER_SIZE];

  int sockfd;
  sockfd = socket(storage.ss_family, SOCK_DGRAM, 0);
  if (sockfd == -1) {
      logExit("socket");
  }

  struct sockaddr *addr = (struct sockaddr *)(&storage);

  char addrStr[BUFFER_SIZE];
  addrToStr(addr, addrStr, BUFFER_SIZE);

  strcpy(buffer, "Hello Server\n");
  sendto(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&addr, sizeof(addr));
  printf("[+]Data Send: %s", buffer);

  return 0;
}

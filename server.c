#include "common.h"

int main(int argc, char **argv){
  if(argc != 3){
    usage(TRUE, argv);
  }

  struct sockaddr_storage storage;
  if (serverSockaddrInit(argv[1], argv[2], &storage) != 0) {
      usage(TRUE, argv);
  }

  int port = atoi(argv[2]);
  int sockfd;
  struct sockaddr_in si_me, si_other;
  char buffer[BUFFER_SIZE];
  socklen_t addr_size;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);

  memset(&si_me, '\0', sizeof(si_me));
  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(port);
  si_me.sin_addr.s_addr = inet_addr("127.0.0.1");

  bind(sockfd, (struct sockaddr*)&si_me, sizeof(si_me));
  addr_size = sizeof(si_other);
  recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)& si_other, &addr_size);
  printf("[+]Data Received: %s", buffer);

  return 0;

}

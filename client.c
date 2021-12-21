#include "common.h"

int main(int argc, char **argv){

  int port = atoi(argv[1]);
  int sockfd;
  struct sockaddr_in serverAddr;
  char buffer[BUFFER_SIZE];
  //socklen_t addr_size;

  sockfd = socket(PF_INET, SOCK_DGRAM, 0);
  memset(&serverAddr, '\0', sizeof(serverAddr));

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  strcpy(buffer, "Hello Server\n");
  sendto(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
  printf("[+]Data Send: %s", buffer);

  return 0;
}

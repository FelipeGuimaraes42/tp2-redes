#include "common.h"

void logExit(const char *msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

// Used if user tries to run the program with a different number of params
void usage(int option, char **argv){
    // Server usage
    if (option){
        printf("usage: %s <v4|v6> <server port>\n", argv[0]);
        printf("example: %s v4 9000\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // Client Usage
    printf("usage: %s <server IP> <server port> start\n", argv[0]);
    printf("example: %s 127.0.0.1 9000 start\n", argv[0]);
    exit(EXIT_FAILURE);
}

int addrParse(const char *addrStr, const char *portStr,
              struct sockaddr_storage *storage) {
    if (addrStr == NULL || portStr == NULL) {
        return -1;
    }

    uint16_t port = (uint16_t)atoi(portStr); // unsigned short
    if (port == 0) {
        return -1;
    }
    port = htons(port); // host to network short

    struct in_addr inaddr4; // 32-bit IP address
    if (inet_pton(AF_INET, addrStr, &inaddr4)) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;
        return 0;
    }

    struct in6_addr inaddr6; // 128-bit IPv6 address
    if (inet_pton(AF_INET6, addrStr, &inaddr6)) {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        // addr6->sin6_addr = inaddr6
        memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6));
        return 0;
    }

    return -1;
}

void addrToStr(const struct sockaddr *addr, char *str, size_t strSize) {
    int version;
    char addrStr[INET6_ADDRSTRLEN + 1] = "";
    uint16_t port;

    if (addr->sa_family == AF_INET) {
        version = 4;
        struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
        if (!inet_ntop(AF_INET, &(addr4->sin_addr), addrStr,
                       INET6_ADDRSTRLEN + 1)) {
            logExit("ntop");
        }
        port = ntohs(addr4->sin_port); // network to host short
    } else if (addr->sa_family == AF_INET6) {
        version = 6;
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;
        if (!inet_ntop(AF_INET6, &(addr6->sin6_addr), addrStr,
                       INET6_ADDRSTRLEN + 1)) {
            logExit("ntop");
        }
        port = ntohs(addr6->sin6_port); // network to host short
    } else {
        logExit("unknown protocol family.");
    }
    if (str) {
        snprintf(str, strSize, "IPv%d %s %hu", version, addrStr, port);
    }
}

int serverSockaddrInit(const char *proto, const char *portStr,
                         struct sockaddr_storage *storage) {
    uint16_t port = (uint16_t)atoi(portStr); // unsigned short
    if (port == 0) {
        return -1;
    }
    port = htons(port); // host to network short

    memset(storage, 0, sizeof(*storage));
    if (0 == strcmp(proto, "v4")) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_addr.s_addr = INADDR_ANY;
        addr4->sin_port = port;
        return 0;
    } else if (0 == strcmp(proto, "v6")) {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_addr = in6addr_any;
        addr6->sin6_port = port;
        return 0;
    } else {
        return -1;
    }
}
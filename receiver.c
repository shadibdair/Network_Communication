#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int sockfd;
    char buffer[1024];
    struct sockaddr_in saddr, client_addr;
    socklen_t addr_size = sizeof(client_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket error");
        exit(1);
    }

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(55555);
    saddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
        perror("Bind error");
        exit(1);
    }

    int data_size = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &addr_size);
    if (data_size < 0) {
        perror("Recv error");
        exit(1);
    }

    buffer[data_size] = '\0';
    printf("Received message: %s\n", buffer);

    char *response = "Received";
    sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));

    close(sockfd);
    return 0;
}

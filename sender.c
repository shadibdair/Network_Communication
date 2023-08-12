#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int sockfd;
    struct sockaddr_in dest_addr;

    char *data = "This is a demonstration!";
    char buffer[1024];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket error");
        exit(1);
    }

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(55555);
    dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    sendto(sockfd, data, strlen(data), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

    socklen_t addr_size = sizeof(dest_addr);
    int data_size = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&dest_addr, &addr_size);
    if (data_size < 0) {
        perror("Recv error");
        exit(1);
    }

    buffer[data_size] = '\0';
    printf("Received response: %s\n", buffer);

    close(sockfd);
    return 0;
}

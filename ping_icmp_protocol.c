#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PACKET_SIZE 64

struct icmphdr {
    uint8_t type; /* message type */
    uint8_t code; /* type sub-code */
    uint16_t checksum;
    union {
        struct {
            uint16_t id;
            uint16_t sequence;
        } echo; /* echo datagram */
        uint32_t gateway; /* gateway address */
        struct {
            uint16_t __unused;
            uint16_t mtu;
        } frag; /* path mtu discovery */
    } un;
};

unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2) {
        sum += *buf++;
    }

    if (len == 1) {
        sum += *(unsigned char *)buf;
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <IP Address>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &addr.sin_addr);

    char packet[PACKET_SIZE];
    struct icmphdr *icmp_hdr = (struct icmphdr *)packet;
    icmp_hdr->type = 8; // Echo request
    icmp_hdr->code = 0;
    icmp_hdr->checksum = 0;
    icmp_hdr->un.echo.id = getpid();
    icmp_hdr->un.echo.sequence = 1;

    icmp_hdr->checksum = checksum(packet, PACKET_SIZE);

    sendto(sock, packet, sizeof(packet), 0, (struct sockaddr *)&addr, sizeof(addr));

    char buffer[PACKET_SIZE];
    struct sockaddr_in r_addr;
    socklen_t r_addr_len = sizeof(r_addr);

    if (recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&r_addr, &r_addr_len) > 0) {
        printf("Received reply from %s\n", inet_ntoa(r_addr.sin_addr));
    } else {
        printf("No reply received\n");
    }

    close(sock);

    return 0;
}
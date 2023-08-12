#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PACKET_SIZE 64

// ICMP header structure definition
struct icmphdr {
    uint8_t type; /* message type, e.g., echo request, echo reply */
    uint8_t code; /* type sub-code */
    uint16_t checksum; /* checksum of the ICMP header and data */
    union {
        struct {
            uint16_t id; /* identifier to aid in matching requests and replies */
            uint16_t sequence; /* sequence number to match requests and replies */
        } echo; /* echo datagram */
        uint32_t gateway; /* gateway address */
        struct {
            uint16_t __unused;
            uint16_t mtu;
        } frag; /* path mtu discovery */
    } un;
};

// Function to calculate the checksum
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
    // Check for correct number of arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <IP Address>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Create a raw socket
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Define destination address
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &addr.sin_addr);

    // Define and initialize ICMP packet
    char packet[PACKET_SIZE];
    struct icmphdr *icmp_hdr = (struct icmphdr *)packet;
    icmp_hdr->type = 8; // Echo request
    icmp_hdr->code = 0; // Code 0 for ICMP Echo request
    icmp_hdr->checksum = 0; // Initial checksum value
    icmp_hdr->un.echo.id = getpid(); // Process ID
    icmp_hdr->un.echo.sequence = 1; // Sequence number

    // Calculate checksum for the packet
    icmp_hdr->checksum = checksum(packet, PACKET_SIZE);

    // Send the packet to the destination address
    sendto(sock, packet, sizeof(packet), 0, (struct sockaddr *)&addr, sizeof(addr));

    // Buffer to hold the response
    char buffer[PACKET_SIZE];
    struct sockaddr_in r_addr;
    socklen_t r_addr_len = sizeof(r_addr);

    // Receive the response and print it
    if (recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&r_addr, &r_addr_len) > 0) {
        printf("Received reply from %s\n", inet_ntoa(r_addr.sin_addr));
    } else {
        printf("No reply received\n");
    }

    // Close the socket
    close(sock);

    return 0;
}

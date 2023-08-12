#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/select.h>

#define MAX_HOPS 30

// Function to calculate ICMP checksum
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

// Function to wait for a reply from the target
int wait_for_reply(int sockfd, int seconds) {
    fd_set readfds;
    struct timeval tv;

    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);

    tv.tv_sec = seconds;
    tv.tv_usec = 0;

    return select(sockfd + 1, &readfds, NULL, NULL, &tv);
}

// Function to send a probe (ICMP Echo Request) to the target
void send_probe(int sockfd, struct sockaddr_in *target, int ttl) {
    struct icmp icmp_hdr;
    icmp_hdr.icmp_type = ICMP_ECHO;
    icmp_hdr.icmp_code = 0;
    icmp_hdr.icmp_id = getpid();
    icmp_hdr.icmp_seq = ttl;
    icmp_hdr.icmp_cksum = 0;
    icmp_hdr.icmp_cksum = checksum(&icmp_hdr, sizeof(icmp_hdr));

    setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
    sendto(sockfd, &icmp_hdr, sizeof(icmp_hdr), 0, (struct sockaddr *)target, sizeof(*target));
}

int main(int argc, char *argv[]) {
    // Check command-line arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <IP Address>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Initialize target address structure
    struct sockaddr_in target;
    target.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &target.sin_addr);

    // Create raw socket
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    printf("Performing traceroute to %s:\n", argv[1]);

    char buffer[512];
    struct sockaddr_in sender;
    socklen_t sender_len = sizeof(sender);

    // Loop through all hops
    for (int ttl = 1; ttl <= MAX_HOPS; ttl++) {
        send_probe(sockfd, &target, ttl); // Send probe

        if (wait_for_reply(sockfd, 1) > 0) {
            int bytes_read = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&sender, &sender_len);

            if (bytes_read > 0) {
                struct ip *ip = (struct ip *)buffer;
                struct icmp *icmp = (struct icmp *)(buffer + (ip->ip_hl << 2));

                // Check if reply is from an intermediate router or the final destination
                if (icmp->icmp_type == ICMP_TIMXCEED || (icmp->icmp_type == ICMP_ECHOREPLY && icmp->icmp_id == getpid())) {
                    printf("%d. %s\n", ttl, inet_ntoa(sender.sin_addr));
                    if (icmp->icmp_type == ICMP_ECHOREPLY) {
                        break; // Destination reached
                    }
                }
            }
        } else {
            printf("%d. *\n", ttl); // Indicate a timeout for this hop
        }
    }

    // Close socket
    close(sockfd);
    return 0;
}

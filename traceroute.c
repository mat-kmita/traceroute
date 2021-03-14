#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>

#include "utils.h"
#include "packets.h"

int main(int argc, char **argv) {
    char dest_ip_str[20];
    int dest_ip_int;
    uint8_t reached_dest = 0;

    if(argc != 2) {
        fprintf(stderr, "Invalid input!\n");
        exit(EXIT_FAILURE); 
    }

    strncpy(dest_ip_str, argv[1], 20);
    
    if(inet_pton(AF_INET, dest_ip_str, (void *)&dest_ip_int) != 1) {
        fprintf(stderr, "Invalid IP address!\n)");
        exit(EXIT_FAILURE);
    }

    int socket_fd;

    if((socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
        printf("Socket error!\n");
        exit(1);
    }

    uint16_t pid = getpid();
    uint16_t npid = htons(getpid());

    int ttl = 1;
    uint16_t seq_nmb = 1;
    uint16_t min_seq, max_seq;
    while( ttl <= 30 ) {
        if(reached_dest) break;

        struct sockaddr_in receiver;
        memset(&receiver, 0, sizeof(receiver));
        inet_pton(AF_INET, dest_ip_str, &receiver.sin_addr);
        receiver.sin_family = AF_INET;

        setsockopt(socket_fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));

        min_seq = seq_nmb;  
        for(int i=0; i<3; i++) {
            struct icmp header;
            create_icmp_echo_packet(&header, npid, htons(seq_nmb));

            sendto(socket_fd, &header, sizeof(header), 0, (struct sockaddr*)&receiver, sizeof(receiver));
            seq_nmb++;
        }
        max_seq = seq_nmb;

        fd_set readfds;
        struct timeval select_time;
        select_time.tv_sec = 1;
        select_time.tv_usec = 0;

        FD_ZERO(&readfds);
        FD_SET(socket_fd, &readfds);

        uint8_t received = 0;
        struct in_addr response_addrs[3];
        unsigned long rrts[3];

        while(received < 3) {
            int select_val;
            if( (select_val = select(socket_fd+1, &readfds, NULL, NULL, &select_time)) == -1) {
                fprintf(stderr, "Select error!\n");
                exit(1);
            } else if(select_val) {
                struct sockaddr_in sender;
                socklen_t sender_size = sizeof(sender);
                uint8_t buffer[IP_MAXPACKET];
                uint8_t *icmp_packet;
                ssize_t read_size;
                uint8_t response_type;
                uint16_t id, seq;

                while(( read_size = recvfrom(socket_fd, &buffer, IP_MAXPACKET, MSG_DONTWAIT, (struct sockaddr*)&sender, &sender_size)) > 0) {
                    icmp_packet = get_icmp_from_ip(buffer);
                    
                    if((response_type = read_icmp_response(icmp_packet, &id, &seq)));

                    if(id != pid || seq < min_seq || seq >= max_seq) {
                        continue;
                    }
                    if(response_type == 0) reached_dest = 1;

                    rrts[received] = 1000 - select_time.tv_usec / 1000;
                    response_addrs[received] = sender.sin_addr;
                    received++;
                }
            } else {
                break;
            }
        }
        print_addresses(ttl, response_addrs, rrts, received);
        ttl++;
    }

    return 0;
}

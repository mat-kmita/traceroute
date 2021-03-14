#ifndef packets_h
#define packets_h

#include <stdint.h>
#include <netinet/ip_icmp.h>

uint8_t *get_icmp_from_ip(uint8_t *ip_buffer);
void create_icmp_echo_packet(struct icmp *header, uint16_t id, uint16_t seq);
uint16_t compute_icmp_checksum (const void *buff, int length);
int8_t read_icmp_response(uint8_t *icmp_header, uint16_t *id, uint16_t *seq);

#endif
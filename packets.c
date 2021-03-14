#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "packets.h"

void create_icmp_echo_packet(struct icmp *header, uint16_t id, uint16_t seq) {
    header->icmp_type = ICMP_ECHO;
    header->icmp_code = 0;
    header->icmp_hun.ih_idseq.icd_id = id;
    header->icmp_hun.ih_idseq.icd_seq = seq;
    header->icmp_cksum = 0;
    header->icmp_cksum = compute_icmp_checksum ( (uint16_t*)header, sizeof( (struct icmp) *header));
}

uint16_t compute_icmp_checksum (const void *buff, int length) {
	uint32_t sum;
	const uint16_t* ptr = buff;
	assert (length % 2 == 0);
	for (sum = 0; length > 0; length -= 2)
		sum += *ptr++;
	sum = (sum >> 16) + (sum & 0xffff);
	return (uint16_t)(~(sum + (sum >> 16)));
}

inline uint8_t *get_icmp_from_ip(uint8_t *ip_buffer) {
    return ip_buffer + 4 * ((struct ip*)ip_buffer)->ip_hl;
}

int8_t read_icmp_response(uint8_t *icmp_header, uint16_t *id, uint16_t *seq) {
    uint8_t resp_type = ((struct icmp*)icmp_header)->icmp_type;
    uint16_t id_resp, seq_resp;

    if(resp_type == 11) {
        uint8_t *req_in_resp = icmp_header + 8;
        uint8_t *icmp_req_in_resp = req_in_resp + 4 * ((struct ip*)req_in_resp)->ip_hl;
        id_resp = ntohs(((struct icmp*)icmp_req_in_resp)->icmp_id);
        seq_resp = ntohs(((struct icmp*)icmp_req_in_resp)->icmp_seq);
    } else if(resp_type == 0 ) {
        id_resp = ntohs(((struct icmp*)icmp_header)->icmp_id);
        seq_resp = ntohs(((struct icmp*)icmp_header)->icmp_seq);
    } else {
        return -1;
    }

    *id = id_resp;
    *seq = seq_resp;

    return resp_type;
}
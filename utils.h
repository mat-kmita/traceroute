#ifndef utils_h
#define utils_h

#include <stdint.h>
#include <arpa/inet.h>

void print_addresses(uint8_t ttl, struct in_addr *addresses, unsigned long *times, size_t data_size);

#endif
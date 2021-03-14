#include <stdio.h>
#include <netdb.h>

#include "utils.h"

void print_addresses(uint8_t ttl, struct in_addr addresses[], unsigned long times[], size_t data_size) {
    printf("%d. ", ttl);

    if(data_size == 0) {
        printf("*\n");
        return;
    }

    unsigned long avg_rrt = 0;

    for(size_t i=0; i<data_size; i++) {
        avg_rrt += times[i]/data_size;

        uint8_t repeated = 0;
        for(size_t j=0; j<i && !repeated; j++) {
            if(addresses[j].s_addr == addresses[i].s_addr)
                repeated = 1;
        }

        if(repeated) continue;

        char ip_str[16];

        if(inet_ntop(AF_INET, &(addresses[i].s_addr), ip_str, sizeof(ip_str)) == NULL) {
            fprintf(stderr, "Can't convert IP from numerical to string!\n");
        }

        struct hostent *q = gethostbyaddr(&addresses[i], sizeof(addresses[i]), AF_INET);  
                        
        if(q != NULL) 
            printf("%s (%s)", q->h_name, ip_str);
        else
            printf("%s", ip_str);  
        }

    if(data_size != 3)
        printf(" ???");
    else
        printf(" %ldms", avg_rrt);

    printf("\n");
}


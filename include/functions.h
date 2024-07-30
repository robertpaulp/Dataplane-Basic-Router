#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "lib.h"

int check_mac(uint8_t *mac1, int interface);
int check_ip(uint32_t ip1, int interface);
int check_ip_checksum(struct iphdr *ip_hdr);
int check_ip_ttl(struct iphdr *ip_hdr);
void ip_forward(struct ether_header *eth_hdr, struct route_table_entry *route, struct arp_table_entry *mac, char *buf, size_t len);

#endif
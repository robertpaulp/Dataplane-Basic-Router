#include "cons.h"
#include "functions.h"
#include "queue.h"

#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <unistd.h>
#include <asm/byteorder.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int check_mac(uint8_t *mac_dhost, int interface)
{
    uint8_t mac2[6];
    uint8_t broadcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    get_interface_mac(interface, mac2);

    if (memcmp(mac_dhost, mac2, 6) == 0) {
        return 0;
    }
    if (memcmp(mac_dhost, broadcast, 6) == 0) {
        return 0;
    }

    return -1;
}

int check_ip(uint32_t ip_daddr, int interface)
{
    char *ip = get_interface_ip(interface);
    uint32_t ip_router = inet_addr(ip);

    if (ip_daddr == ip_router) {
        return 0;
    }

    return -1;
}

int check_ip_checksum(struct iphdr *ip_hdr)
{
    ip_hdr->check = ntohs(ip_hdr->check);
	uint16_t ip_checksum = ip_hdr->check;
	ip_hdr->check = 0;
	uint16_t check = checksum((uint16_t *)ip_hdr, sizeof(struct iphdr));
	if (check == ip_checksum) {
        return 0;
    }

    return -1;
}

int check_ip_ttl(struct iphdr *ip_hdr)
{
    if (ip_hdr->ttl > 1) {
        return 0;
    }
    
    return -1;
}

void ip_forward(struct ether_header *eth_hdr, struct route_table_entry *route, struct arp_table_entry *mac, char *buf, size_t len)
{
	get_interface_mac(route->interface, eth_hdr->ether_shost);
	memcpy(eth_hdr->ether_dhost, mac->mac, sizeof(mac->mac));

	send_to_link(route->interface, buf, len);
}
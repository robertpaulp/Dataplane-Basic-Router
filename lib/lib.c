#include "lib.h"

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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int interfaces[ROUTER_NUM_INTERFACES];

int get_sock(const char *if_name)
{
	int res;
	int s = socket(AF_PACKET, SOCK_RAW, 768);
	DIE(s == -1, "socket");

	struct ifreq intf;
	strcpy(intf.ifr_name, if_name);
	res = ioctl(s, SIOCGIFINDEX, &intf);
	DIE(res, "ioctl SIOCGIFINDEX");

	struct sockaddr_ll addr;
	memset(&addr, 0x00, sizeof(addr));
	addr.sll_family = AF_PACKET;
	addr.sll_ifindex = intf.ifr_ifindex;

	res = bind(s, (struct sockaddr *)&addr , sizeof(addr));
	DIE(res == -1, "bind");
	return s;
}

int send_to_link(int intidx, char *frame_data, size_t length)
{
	/*
	 * Note that "buffer" should be at least the MTU size of the 
	 * interface, eg 1500 bytes 
	 */
	int ret;
	ret = write(interfaces[intidx], frame_data, length);
	DIE(ret == -1, "write");
	return ret;
}

ssize_t receive_from_link(int intidx, char *frame_data)
{
	ssize_t ret;
	ret = read(interfaces[intidx], frame_data, MAX_PACKET_LEN);
	return ret;
}

int socket_receive_message(int sockfd, char *frame_data, size_t *len)
{
	/*
	 * Note that "buffer" should be at least the MTU size of the
	 * interface, eg 1500 bytes
	 * */
	int ret = read(sockfd, frame_data, MAX_PACKET_LEN);
	DIE(ret < 0, "read");
	*len = ret;
	return 0;
}

int recv_from_any_link(char *frame_data, size_t *length) {
	int res;
	fd_set set;

	FD_ZERO(&set);
	while (1) {
		for (int i = 0; i < ROUTER_NUM_INTERFACES; i++) {
			FD_SET(interfaces[i], &set);
		}

		res = select(interfaces[ROUTER_NUM_INTERFACES - 1] + 1, &set,
				NULL, NULL, NULL);
		DIE(res == -1, "select");

		for (int i = 0; i < ROUTER_NUM_INTERFACES; i++) {
			if (FD_ISSET(interfaces[i], &set)) {
				ssize_t ret = receive_from_link(i, frame_data);
				DIE(ret < 0, "receive_from_link");
				*length = ret;
				return i;
			}
		}
	}

	return -1;
}

char *get_interface_ip(int interface)
{
	struct ifreq ifr;
	int ret;
	if (interface == 0)
		sprintf(ifr.ifr_name, "rr-0-1");
	else {
		sprintf(ifr.ifr_name, "r-%u", interface - 1);
	}
	ret = ioctl(interfaces[interface], SIOCGIFADDR, &ifr);
	DIE(ret == -1, "ioctl SIOCGIFADDR");
	return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}

void get_interface_mac(int interface, uint8_t *mac)
{
	struct ifreq ifr;
	int ret;
	if (interface == 0)
		sprintf(ifr.ifr_name, "rr-0-1");
	else {
		sprintf(ifr.ifr_name, "r-%u", interface - 1);
	}
	ret = ioctl(interfaces[interface], SIOCGIFHWADDR, &ifr);
	DIE(ret == -1, "ioctl SIOCGIFHWADDR");
	memcpy(mac, ifr.ifr_addr.sa_data, 6);
}

static int hex2num(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;

	return -1;
}

int hex2byte(const char *hex)
{
	int a, b;
	a = hex2num(*hex++);
	if (a < 0)
		return -1;
	b = hex2num(*hex++);
	if (b < 0)
		return -1;

	return (a << 4) | b;
}

int hwaddr_aton(const char *txt, uint8_t *addr)
{
	int i;
	for (i = 0; i < 6; i++) {
		int a, b;
		a = hex2num(*txt++);
		if (a < 0)
			return -1;
		b = hex2num(*txt++);
		if (b < 0)
			return -1;
		*addr++ = (a << 4) | b;
		if (i < 5 && *txt++ != ':')
			return -1;
	}
	return 0;
}

void init(int argc, char *argv[])
{
	for (int i = 0; i < argc; ++i) {
		printf("Setting up interface: %s\n", argv[i]);
		interfaces[i] = get_sock(argv[i]);
	}
}


uint16_t checksum(uint16_t *data, size_t length)
{
	unsigned long checksum = 0;
	uint16_t extra_byte;
	while (length > 1) {
		checksum += ntohs(*data++);
		length -= 2;
	}
	if (length) {
		*(uint8_t *)&extra_byte = *(uint8_t *)data;
		checksum += extra_byte;
	}

	checksum = (checksum >> 16) + (checksum & 0xffff);
	checksum += (checksum >>16);
	return (uint16_t)(~checksum);
}

int read_rtable(const char *path, struct route_table_entry *rtable, struct trie_node *root)
{
	FILE *fp = fopen(path, "r");
	int j = 0, i;
	char *p, line[64];

	while (fgets(line, sizeof(line), fp) != NULL) {
		p = strtok(line, " .");
		i = 0;
		while (p != NULL) {
			if (i < 4)
				*(((unsigned char *)&rtable[j].prefix)  + i % 4) = (unsigned char)atoi(p);

			if (i >= 4 && i < 8)
				*(((unsigned char *)&rtable[j].next_hop)  + i % 4) = atoi(p);

			if (i >= 8 && i < 12)
				*(((unsigned char *)&rtable[j].mask)  + i % 4) = atoi(p);

			if (i == 12)
				rtable[j].interface = atoi(p);
			p = strtok(NULL, " .");
			i++;
		}
		insert_trie(root, &rtable[j]);
		j++;
	}
	return j;
}

struct route_table_entry *get_best_route(uint32_t dest_ip, struct trie_node *root)
{
	struct route_table_entry *route = search_trie(root, dest_ip);
	if (route) {
		return route;
	}

	return NULL;
}

int parse_arp_table(char *path, struct arp_table_entry *arp_table)
{
	FILE *f;
	f = fopen(path, "r");
	DIE(f == NULL, "Failed to open %s", path);
	char line[100];
	int i = 0;
	for(i = 0; fgets(line, sizeof(line), f); i++) {
		char ip_str[50], mac_str[50];
		sscanf(line, "%s %s", ip_str, mac_str);
		arp_table[i].ip = inet_addr(ip_str);
		int rc = hwaddr_aton(mac_str, arp_table[i].mac);
		DIE(rc < 0, "invalid MAC");
	}
	fclose(f);
	return i;
}

struct trie_node *get_trie_node(void)
{
    struct trie_node *node = (struct trie_node *)malloc(sizeof(struct trie_node));
    if (node) {
        node->is_end_of_word = false;
        for (int i = 0; i < CHILDREN; i++) {
            node->children[i] = NULL;
        }
        node->data = NULL;
    }

    return node;
}

void insert_trie(struct trie_node *root, struct route_table_entry *key)
{
	struct trie_node *curr_node = root;
	int depth = sizeof(uint32_t) * 8;
	key->prefix = ntohl(key->prefix);
	key->mask = ntohl(key->mask);
	
	for (int i = depth - 1; i >= 0; i--) {
		int bit = (key->prefix >> i) & 1;
		int mask_bit = (key->mask >> i) & 1;
		
		if (mask_bit == 0) {
			break;
		}

		if (!curr_node->children[bit]) {
			curr_node->children[bit] = get_trie_node();
		}

		curr_node = curr_node->children[bit];
	}

	curr_node->is_end_of_word = true;
	curr_node->data = key;	
}

struct route_table_entry *search_trie(struct trie_node *root, uint32_t key)
{
	key = ntohl(key);
	struct trie_node *curr_node = root;
	int depth = sizeof(uint32_t) * 8;

	for (int i = depth - 1; i >= 0; i--) {
		int bit = (key >> i) & 1;
		if (!curr_node->children[bit]) {
			break;
		}
		curr_node = curr_node->children[bit];
	}

	if (curr_node != NULL && curr_node->is_end_of_word) {
		return curr_node->data;
	}
	
	return NULL;
}

struct arp_table_entry *get_mac_entry(uint32_t given_ip, struct arp_table_entry *arptable, int arptable_len)
{

	for (int i = 0; i < arptable_len; i++) {
		if (arptable[i].ip == given_ip) {
			return &arptable[i];
		}
	}

	return NULL;
}

void send_icmp_reply(uint8_t type, uint8_t code, struct ether_header *eth_hdr, struct iphdr *ip_hdr, struct icmphdr *icmp_hdr, char *buf, int interface)
{
	/*
	Echo reply -> type 0
				  code 0
				  checksum header
				  id
				  sequence
	*/

	icmp_hdr->type = type;
	icmp_hdr->code = code;
	icmp_hdr->checksum = 0;
	icmp_hdr->checksum = htons(checksum((uint16_t *)icmp_hdr, sizeof(struct icmphdr)));
	
	uint8_t aux[6];
	memcpy(aux, eth_hdr->ether_dhost, 6);
	memcpy(eth_hdr->ether_dhost, eth_hdr->ether_shost, 6);
	memcpy(eth_hdr->ether_shost, aux, 6);

	uint32_t aux_ip = ip_hdr->daddr;
	ip_hdr->daddr = ip_hdr->saddr;
	ip_hdr->saddr = aux_ip;

	send_to_link(interface, buf, ETH_IPV4_ICMP_HDR_SIZE);
}

void send_icmp_errors(uint8_t type, uint8_t code, struct ether_header *eth_hdr, struct iphdr *ip_hdr, struct icmphdr *icmp_hdr, char *buf, int len, int interface)
{
	/*
	Destination unreachable -> type 3
							   code 0
							   checksum header
							   unused
							   64 bits of IP header
	
	Time exceeded -> type 11
					 code 0
					 checksum header
					 unused
					 64 bits of IP header
	*/
	char original_data[ICMP_ERROR_EXTRA_BYTES];
	memcpy(original_data, buf + sizeof(struct ether_header), ICMP_ERROR_EXTRA_BYTES);

	icmp_hdr->type = type;
	icmp_hdr->code = code;
	icmp_hdr->checksum = 0;
	memset(&icmp_hdr->un, 0, sizeof(icmp_hdr->un));

	uint8_t aux[6];
	memcpy(aux, eth_hdr->ether_dhost, 6);
	memcpy(eth_hdr->ether_dhost, eth_hdr->ether_shost, 6);
	memcpy(eth_hdr->ether_shost, aux, 6);

	uint32_t aux_ip = ip_hdr->daddr;
	ip_hdr->daddr = ip_hdr->saddr;
	ip_hdr->saddr = aux_ip;
	ip_hdr->ttl = IP_TTL;
	ip_hdr->check = 0;
	ip_hdr->tot_len = htons(IP_TOT_LEN_ICMP_ERROR);
	ip_hdr->protocol = IP_PROTOCOL_ICMP;
	ip_hdr->check = htons(checksum((uint16_t *)ip_hdr, sizeof(struct iphdr)));


	icmp_hdr->checksum = htons(checksum((uint16_t *)icmp_hdr, ICMP_ERROR_HDR_SIZE));
	memcpy(buf + ETH_IPV4_ICMP_HDR_SIZE, original_data, sizeof(original_data));
	
	send_to_link(interface, buf, ETH_IPV4_HDR_SIZE + ICMP_ERROR_HDR_SIZE);
}

void send_arp_reply(struct ether_header *eth_hdr, struct arp_header *arp_hdr, char *buf,  int interface)
{ // 				send_arp_reply(arp_hdr->tpa, arp_hdr->spa, arp_hdr->sha, interface);
	/*
	ARP request -> opcode 2
				   sender MAC
				   sender IP
				   target MAC
				   target IP
	*/
	uint8_t s_mac[6];
	uint32_t spa_aux = arp_hdr->spa;
	get_interface_mac(interface, s_mac);

	arp_hdr->htype = htons(ARP_HTYPE_ETHERNET);
	arp_hdr->ptype = htons(ARP_PTYPE_IPV4);
	arp_hdr->hlen = ARP_ETH_HLEN;
	arp_hdr->plen = ARP_IPV4_PLEN;
	arp_hdr->op = htons(ARP_REPLY);
	memcpy(arp_hdr->tha, arp_hdr->sha, 6);
	memcpy(arp_hdr->sha, s_mac, 6);
	arp_hdr->spa = arp_hdr->tpa;
	arp_hdr->tpa = spa_aux;

	memcpy(eth_hdr->ether_dhost, arp_hdr->tha, 6);
	memcpy(eth_hdr->ether_shost, s_mac, 6);
	eth_hdr->ether_type = htons(ETH_TYPE_ARP);

	memcpy(buf, eth_hdr, sizeof(struct ether_header));
	memcpy(buf + sizeof(struct ether_header), arp_hdr, sizeof(struct arp_header));

	send_to_link(interface, buf, ETH_ARP_HDR_SIZE);
}

void send_arp_request(uint32_t dest_ip, int interface)
{					// route->next_hop, route->interface
	/*
	ARP request -> opcode 1
				   sender MAC
				   sender IP
				   target MAC
				   target IP
	*/

	char buf[ETH_ARP_HDR_SIZE];
	char BROADCAST_MAC[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	uint8_t s_mac[6];
	get_interface_mac(interface, s_mac);

	struct ether_header *eth_hdr = (struct ether_header *)(malloc(sizeof(struct ether_header)));
	struct arp_header *arp_hdr = (struct arp_header *)(malloc(sizeof(struct arp_header)));

	arp_hdr->htype = htons(ARP_HTYPE_ETHERNET);
	arp_hdr->ptype = htons(ARP_PTYPE_IPV4);
	arp_hdr->hlen = ARP_ETH_HLEN;
	arp_hdr->plen = ARP_IPV4_PLEN;
	arp_hdr->op = htons(ARP_REQUEST);
	memcpy(arp_hdr->sha, s_mac, 6);
	arp_hdr->spa = inet_addr(get_interface_ip(interface));
	memset(arp_hdr->tha, 0, 6);
	arp_hdr->tpa = dest_ip;

	memcpy(eth_hdr->ether_dhost, BROADCAST_MAC, 6);
	memcpy(eth_hdr->ether_shost, s_mac, 6);
	eth_hdr->ether_type = htons(ETH_TYPE_ARP);

	memcpy(buf, eth_hdr, sizeof(struct ether_header));
	memcpy(buf + sizeof(struct ether_header), arp_hdr, sizeof(struct arp_header));

	send_to_link(interface, buf, ETH_ARP_HDR_SIZE);
}
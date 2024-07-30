#ifndef _SKEL_H_
#define _SKEL_H_

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cons.h"
#include "protocols.h"

#define MAX_PACKET_LEN 1600
#define ROUTER_NUM_INTERFACES 3


/*
 * @brief Sends a packet on a specific interface.
 *
 * @param interface - index of the output interface
 * @param frame_data - region of memory in which the data will be copied; should
 *        have at least MAX_PACKET_LEN bytes allocated
 * @param length - will be set to the total number of bytes received.
 * Returns: the interface it has been received from.
 */
int send_to_link(int interface, char *frame_data, size_t length);

/*
 * @brief Receives a packet. Blocking function, blocks if there is no packet to
 * be received.
 *
 * @param frame_data - region of memory in which the data will be copied; should
 *        have at least MAX_PACKET_LEN bytes allocated 
 * @param length - will be set to the total number of bytes received.
 * Returns: the interface it has been received from.
 */
int recv_from_any_link(char *frame_data, size_t *length);

/* Route table entry */
struct route_table_entry {
	uint32_t prefix;
	uint32_t next_hop;
	uint32_t mask;
	int interface;
} __attribute__((packed));

/* ARP table entry when skipping the ARP exercise */
struct arp_table_entry {
    uint32_t ip;
    uint8_t mac[6];
};

struct trie_node {
    struct trie_node *children[CHILDREN];
    bool is_end_of_word;
    struct route_table_entry *data;
};

char *get_interface_ip(int interface);

/**
 * @brief Get the interface mac object. The function writes
 * the MAC at the pointer mac. uint8_t *mac should be allocated.
 *
 * @param interface
 * @param mac
 */
void get_interface_mac(int interface, uint8_t *mac);

/**
 * @brief Homework infrastructure function.
 *
 * @param argc
 * @param argv
 */

/**
 * @brief IPv4 checksum per  RFC 791. To compute the checksum
 * of an IP header we must set the checksum to 0 beforehand.
 *
 * also works as ICMP checksum per RFC 792. To compute the checksum
 * of an ICMP header we must set the checksum to 0 beforehand.

 * @param data memory area to checksum
 * @param length in bytes
 */
uint16_t checksum(uint16_t *data, size_t length);

/**
 * hwaddr_aton - Convert ASCII string to MAC address (colon-delimited format)
 * @txt: MAC address as a string (e.g., "00:11:22:33:44:55")
 * @addr: Buffer for the MAC address (ETH_ALEN = 6 bytes)
 * Returns: 0 on success, -1 on failure (e.g., string not a MAC address)
 */
int hwaddr_aton(const char *txt, uint8_t *addr);

/* Populates a route table from file, rtable should be allocated
 * e.g. rtable = malloc(sizeof(struct route_table_entry) * 80000);
 * This function returns the size of the route table.
 */
int read_rtable(const char *path, struct route_table_entry *rtable, struct trie_node *root);

/* Parses a static mac table from path and populates arp_table.
 * arp_table should be allocated and have enough space. This
 * function returns the size of the arp table.
 * */
int parse_arp_table(char *path, struct arp_table_entry *arp_table);

void init(int argc, char *argv[]);

/* Parses a binary trie given as argument and returns the best route
 * for the given destination IP. It's used in the Longest-Prefix-Match algorithm.
 * */
struct route_table_entry *get_best_route(uint32_t dest_ip, struct trie_node *root);

/* Functions to create the trie and search for LPM */
struct trie_node *get_trie_node(void);
void insert_trie(struct trie_node *root, struct route_table_entry *key);
struct route_table_entry *search_trie(struct trie_node *root, uint32_t key);

/* Arp functions */
struct arp_table_entry *get_mac_entry(uint32_t given_ip, struct arp_table_entry *arptable, int arptable_len);
void send_arp_reply(struct ether_header *eth_hdr, struct arp_header *arp_hdr, char *buf,  int interface);
void send_arp_request(uint32_t dest_ip, int interface);

/* IMCP functions */
void send_icmp_reply(uint8_t type, uint8_t code, struct ether_header *eth_hdr, struct iphdr *ip_hdr, struct icmphdr *icmp_hdr, char *buf, int interface);
void send_icmp_errors(uint8_t type, uint8_t code, struct ether_header *eth_hdr, struct iphdr *ip_hdr, struct icmphdr *icmp_hdr, char *buf, int len, int interface);

#define DIE(condition, message, ...) \
	do { \
		if ((condition)) { \
			fprintf(stderr, "[(%s:%d)]: " # message "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
			perror(""); \
			exit(1); \
		} \
	} while (0)

#endif /* _SKEL_H_ */

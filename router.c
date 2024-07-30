#include "functions.h"
#include "lib.h"
#include "cons.h"
#include "queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* Routing table */
struct route_table_entry *rtable;
int rtable_len;

/* Arp table */
struct arp_table_entry *arptable;
int arptable_len;

int main(int argc, char *argv[])
{
	char buf[MAX_PACKET_LEN];

	// Do not modify this line
	init(argc - 2, argv + 2);

	/* Prepare the routing table */
	struct trie_node *root = get_trie_node();
	rtable = malloc(sizeof(struct route_table_entry) * 100001);
	DIE(rtable == NULL, "memory");

	rtable_len = read_rtable(argv[1], rtable, root);

	/* Prepare the arp table */
	arptable = malloc(sizeof(struct arp_table_entry) * 100001);
	DIE(arptable == NULL, "memory");

	/* Prepare buffer saver */
	int buf_saver_len = 0;
	char **buf_saver = malloc(sizeof(char *) * 100001);
	DIE(buf_saver == NULL, "memory");

	size_t *buf_len = malloc(sizeof(size_t) * 100001);
	DIE(buf_len == NULL, "memory");

	/* Prepare the queue */
	queue q_packet = queue_create();
	queue q_packet_len = queue_create();
	queue q_packet_route = queue_create();

	/* Prepare the aux queue */
	queue q_packet_aux = queue_create();
	queue q_packet_len_aux = queue_create();
	queue q_packet_route_aux = queue_create();

	while (1) {
		int interface;
		size_t len;

		interface = recv_from_any_link(buf, &len);
		DIE(interface < 0, "recv_from_any_links");

		if (len < sizeof(struct ether_header)) {
			fprintf(stderr, "Error: Packet is shorter than ethernet header\n");
			continue;
		}

		struct ether_header *eth_hdr = (struct ether_header *)buf;

		if (check_mac(eth_hdr->ether_dhost, interface) != 0) {
			fprintf(stderr, "Error: Wrong eth_dhost received\n");
			continue;
		}

		eth_hdr->ether_type = ntohs(eth_hdr->ether_type);
		if (eth_hdr->ether_type == ETH_TYPE_IPV4) {
			if (len < ETH_IPV4_HDR_SIZE) {
				fprintf(stderr, "Error: Packet is shorter than IP header\n");
				continue;
			}

			struct iphdr *ip_hdr = (struct iphdr *)(buf + sizeof(struct ether_header));
			eth_hdr->ether_type = htons(eth_hdr->ether_type);
			if (check_ip(ip_hdr->daddr, interface) != 0) {
				struct icmphdr *icmp_hdr = (struct icmphdr *)(buf + ETH_IPV4_HDR_SIZE);

				/* Checking checksum */
				if (check_ip_checksum(ip_hdr) != 0) {
					fprintf(stderr, "Error: Wrong ip_checksum\n");
					continue;
				}

				/* Checking ttl */
				if (check_ip_ttl(ip_hdr) != 0) {
					fprintf(stderr, "Error: Wrong ttl\n");
					send_icmp_errors(ICMP_TIME_EXCEEDED, ICMP_CODE_0, eth_hdr, ip_hdr, icmp_hdr, buf, len, interface);
					continue;
				}
				ip_hdr->ttl--;

				/* Updating the checksum */
				ip_hdr->check = 0;
				ip_hdr->check = htons(checksum((uint16_t *)ip_hdr, sizeof(struct iphdr)));

				/* Searching for the best route*/
				struct route_table_entry *route = get_best_route(ip_hdr->daddr, root);
				if (route == NULL) {
					fprintf(stderr, "Error: No route found\n");
					send_icmp_errors(ICMP_DEST_UNREACHABLE, ICMP_CODE_0, eth_hdr, ip_hdr, icmp_hdr, buf, len, interface);
					continue;
				}

				/* Get the mac address */
				struct arp_table_entry *mac = get_mac_entry(route->next_hop, arptable, arptable_len);
				if (mac == NULL) {
					// Save the buffer and len
					buf_saver[buf_saver_len] = malloc(len * sizeof(char));
					memcpy(buf_saver[buf_saver_len], buf, len);
					buf_len[buf_saver_len] = len;

					queue_enq(q_packet, buf_saver[buf_saver_len]);
					queue_enq(q_packet_len, &buf_len[buf_saver_len]);
					queue_enq(q_packet_route, route);

					buf_saver_len++;

					/* Send an ARP request */
					send_arp_request(route->next_hop, route->interface);
					continue;
				}

				/* Ip forwarding */
				ip_forward(eth_hdr, route, mac, buf, len);

			} else {
				struct icmphdr *icmp_hdr = (struct icmphdr *)(buf + ETH_IPV4_HDR_SIZE);

				/* Checking checksum */
				if (check_ip_checksum(ip_hdr) != 0) {
					fprintf(stderr, "Error: Wrong ip_checksum\n");
					continue;
				}

				/* Checking ttl */
				if (check_ip_ttl(ip_hdr) != 0) {
					fprintf(stderr, "Error: Wrong ttl\n");
					send_icmp_errors(ICMP_TIME_EXCEEDED, ICMP_CODE_0, eth_hdr, ip_hdr, icmp_hdr, buf, len, interface);
					continue;
				}
				ip_hdr->ttl--;

				/* Update the checksum */
				ip_hdr->check = 0;
				ip_hdr->check = htons(checksum((uint16_t *)ip_hdr, sizeof(struct iphdr)));

				/* Send ICMP reply */
				send_icmp_reply(ICMP_ECHO_REPLY, ICMP_CODE_0, eth_hdr, ip_hdr, icmp_hdr, buf, interface);
			}
		}

		if (eth_hdr->ether_type == ETH_TYPE_ARP) {
			if (len < ETH_ARP_HDR_SIZE) {
				fprintf(stderr, "Error: Packet is shorter than ARP header\n");
				continue;
			}

			struct arp_header *arp_hdr = (struct arp_header *)(buf + sizeof(struct ether_header));
			eth_hdr->ether_type = htons(eth_hdr->ether_type);
			if (ntohs(arp_hdr->op) == ARP_REQUEST) {
				send_arp_reply(eth_hdr, arp_hdr, buf, interface);

			} else if (ntohs(arp_hdr->op) == ARP_REPLY) {
				printf("Received an ARP reply\n");

				/* Add the entry to the arptable */
				arptable[arptable_len].ip = arp_hdr->spa;
				memcpy(arptable[arptable_len].mac, arp_hdr->sha, 6);
				arptable_len++;

				while(!queue_empty(q_packet)) {
					char *packet = queue_deq(q_packet);
					size_t packet_len = *(size_t *)queue_deq(q_packet_len);
					struct route_table_entry *packet_route = (struct route_table_entry *)queue_deq(q_packet_route);
					struct ether_header *packet_eth_hdr = (struct ether_header *)packet;

					struct iphdr *packet_ip_hdr = (struct iphdr *)(packet + sizeof(struct ether_header));

					/* Get the mac address */
					struct arp_table_entry *mac = get_mac_entry(packet_route->next_hop, arptable, arptable_len);
					if (mac == NULL) {
						queue_enq(q_packet_aux, packet);
						queue_enq(q_packet_len_aux, &packet_len);
						queue_enq(q_packet_route_aux, packet_route);
						continue;
					} else {

					/* Ip forwarding */
					ip_forward(packet_eth_hdr, packet_route, mac, packet, packet_len);
					}
				}

				// Copy the aux queues to the main queues
				while(!queue_empty(q_packet_aux)) {
					queue_enq(q_packet, queue_deq(q_packet_aux));
					queue_enq(q_packet_len, queue_deq(q_packet_len_aux));
					queue_enq(q_packet_route, queue_deq(q_packet_route_aux));
				}

			}
		}
	}
}

/* Enthernet Cons */
#define ETH_TYPE_IPV4 0x0800
#define ETH_TYPE_ARP 0x0806

/* Headers Cons */
#define ETH_IPV4_HDR_SIZE (sizeof(struct ether_header) + sizeof(struct iphdr))
#define ETH_IPV4_ICMP_HDR_SIZE (ETH_IPV4_HDR_SIZE + sizeof(struct icmphdr))
#define ICMP_ERROR_HDR_SIZE (sizeof(struct icmphdr) + sizeof(struct iphdr) + 8)
#define ETH_ARP_HDR_SIZE (sizeof(struct ether_header) + sizeof(struct arp_header))

/* Struct Trie Cons*/
#define CHILDREN 2

/* IP Cons */
#define IP_PROTOCOL_ICMP 0x1
#define IP_TOT_LEN_ICMP_ERROR (2 * sizeof(struct iphdr) + sizeof(struct icmphdr) + 8)
#define IP_TTL 128

/* ICMP Cons */
#define ICMP_DEST_UNREACHABLE 3
#define ICMP_TIME_EXCEEDED 11
#define ICMP_CODE_0 0
#define ICMP_ECHO_REPLY 0
#define ICMP_ERROR_EXTRA_BYTES (sizeof(struct iphdr) + 8)

/* ARP Cons */
#define ARP_REQUEST 1
#define ARP_REPLY 2
#define ARP_HTYPE_ETHERNET 1
#define ARP_PTYPE_IPV4 0x0800
#define ARP_ETH_HLEN 6
#define ARP_IPV4_PLEN 4
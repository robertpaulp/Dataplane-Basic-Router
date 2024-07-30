# Dataplane Basic Router

## Overview

This project is a basic router that can forward packets. The router has a routing table (represented as a trie) and uses a Longest Prefix Match algorithm to find the best route for a packet. The router works with IPv4, ICMP and ARP packets.

The topology of the network used during the assignment is :

![Topology](./assignment_requirements/src/topology.png)

## Ethernet

```
+-----------------+------------+-------------+
|     Bytes 0-5   | Bytes 6-11 | Bytes 12-13 |
+------------------------------+-------------+
| Destination MAC | Source MAC |  EtherType  |
+-----------------+------------+-------------+
```
In the ethernet frame, program checks if the packet received has the correct MAC address. If it does, it checks the packet type and branches to the respective function.

## IPv4

```
+----+---------------+---------------+---------------+---------------+
|Word|       1       |       2       |       3       |       4       |
+----+---------------+---------------+---------------+---------------+
|Byte|0|1|2|3|4|5|6|7|8|9|0|1|2|3|4|5|6|7|8|9|0|1|2|3|4|5|6|7|8|9|0|1|
+----+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|   0|Version|  IHL  |Type of Service|          Total Length         |
+----+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|   4|         Identification        |Flags|      Fragment Offset    |
+----+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|   8|  Time to Live |    Protocol   |         Header Checksum       |
+----+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  12|                       Source Address                          |
+----+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  16|                    Destination Address                        |
+----+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```
In the IPv4 packet coding section, we check if the packet is for the router or we need to prepare for forwarding. In the first case we check the checkum, TTL, update checksum and send an ICMP reply to the sender. In the second case we also revise the checksum and TTL. If everything is all right, we verify the routing table and send an ICMP Destination Unreachable if the router doesn't know the destination. If the router knows the destination, we check the arp table and put the packet in a queue or send it directly.

## ARP
```
0        7        15       23       31
+--------+--------+--------+--------+
|       HTYPE     |       PTYPE     |  HTYPE - Format of hardware address
+--------+--------+--------+--------+  PTYPE - Format of protocol address
|  HLEN  |  PLEN  |        OP       |  HLEN - Length of hardware address (6 for MAC)
+--------+--------+--------+--------+  PLEN - Length of protocol address (4 for IP)
|         SHA (bytes 0-3)           |  OP - ARP opcode (command, request or reply)
+--------+--------+--------+--------+  SHA - Sender hardware address
| SHA (bytes 4-5) | SPA (bytes 0-1) |  SPA - Sender IP address
+--------+--------+--------+--------+  THA - Target hardware address
| SPA (bytes 2-3) | THA (bytes 0-1) |  TPA - Target IP address
+--------+--------+--------+--------+
| THA (bytes 2-3) | THA (bytes 4-5) |
+--------+--------+--------+--------+
|         TPA (bytes 0-3)           |
+--------+--------+--------+--------+
```
In the ARP packet, if we received an request, we send an reply. Otherwise, we check all the items in the queue and send the packets with known MAC address. The ARP table is dinamically built, if we don't know the MAC address, we send an ARP request and put the packet in the queue.

## Trie

In order to search the route table more optimally, I implemented a Binary Trie. The search in the trie is O(1). The trie stores the IP adresses in a binary fomat. The leaves store the route_table_entry* and the internal nodes only store the next node.

For more details regarding the implementation of the trie, please refer to [Lim, Hyesook & Kim, Wonjung & Lee, Bomi. (2005). Binary search in a balanced tree for IP address lookup. 490 - 494. 10.1109/HPSR.2005.1503281](https://www.researchgate.net/publication/4171736_Binary_search_in_a_balanced_tree_for_IP_address_lookup).
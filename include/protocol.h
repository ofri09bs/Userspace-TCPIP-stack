#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

// EtherType values
#define ETH_P_IP    0x0800 
#define ETH_P_ARP   0x0806 

// IP Protocol numbers
#define IP_PROTO_ICMP 1

// ARP operation codes
#define ARP_OP_REQUEST 1
#define ARP_OP_REPLY   2

// ICMP message types
#define ICMP_ECHO_REQUEST 8
#define ICMP_ECHO_REPLY   0

// MAC address structure (ethernet address)
typedef struct {
    uint8_t addr_bytes[6];
} __attribute__((packed)) mac_addr;


// Ethernet header structure
typedef struct {
    mac_addr dest_mac;   // Destination MAC address
    mac_addr src_mac;    // Source MAC address
    uint16_t ethertype;  // EtherType field
} __attribute__((packed)) ethernet_header;

// ARP header structure (when ethertype is 0x0806)
typedef struct {
    uint16_t htype;    // Hardware type
    uint16_t ptype;    // Protocol type
    uint8_t hlen;      // Hardware address length
    uint8_t plen;      // Protocol address length
    uint16_t opcode;  // Operation code
    mac_addr sender_mac; // Sender MAC address
    uint32_t sender_ip; // Sender IP address
    mac_addr target_mac; // Target MAC address
    uint32_t target_ip; // Target IP address
} __attribute__((packed)) arp_header;


// IP header structure (when ethertype is 0x0800)
typedef struct {
    uint8_t version_ihl;      // Version and Internet Header Length
    uint8_t tos;              // Type of Service
    uint16_t total_length;    // Total Length
    uint16_t id;             // Unique Identifier for the packet
    uint16_t fragment_offset; // Fragment Offset (packets seperation)
    uint8_t ttl;              // Time to Live
    uint8_t protocol;         // Protocol (e.g., TCP, UDP)
    uint16_t checksum;       // Header checksum
    uint32_t src_ip;         // Source IP address
    uint32_t dest_ip;        // Destination IP address
} __attribute__((packed)) ip_header;

// ICMP header structure
typedef struct {
    uint8_t type;        // ICMP message type
    uint8_t code;       // ICMP message code
    uint16_t checksum;  // ICMP checksum
    uint16_t id;         // Identifier
    uint16_t sequence;   // Sequence number
} __attribute__((packed)) icmp_header;

#endif // PROTOCOL_H
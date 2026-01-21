#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "protocol.h"
#include "utils.h"

void send_icmp_reply(int tap_fd, ethernet_header *eth_hdr) {

    ip_header *ip_request = (ip_header *)((char *)eth_hdr + sizeof(ethernet_header));
    icmp_header *icmp_request = (icmp_header *)((char *)ip_request + sizeof(ip_header));

    uint16_t total_length = ntohs(ip_request->total_length);
    uint16_t data_length = total_length - sizeof(ip_header) - sizeof(icmp_header);

    char response[sizeof(ethernet_header) + sizeof(ip_header) + sizeof(icmp_header) + data_length];
    ethernet_header *resp_eth_hdr = (ethernet_header *)response;
    ip_header *resp_ip_hdr = (ip_header *)(response + sizeof(ethernet_header));
    icmp_header *resp_icmp_hdr = (icmp_header *)(response + sizeof(ethernet_header) + sizeof(ip_header));
    char *resp_data = response + sizeof(ethernet_header) + sizeof(ip_header) + sizeof(icmp_header);

    // Fill Ethernet header
    memcpy(&resp_eth_hdr->dest_mac, &eth_hdr->src_mac, sizeof(mac_addr));
    memcpy(&resp_eth_hdr->src_mac, &eth_hdr->dest_mac, sizeof(mac_addr));
    resp_eth_hdr->ethertype = htons(ETH_P_IP);

    // Fill IP header
    resp_ip_hdr->version_ihl = ip_request->version_ihl;
    resp_ip_hdr->tos = 0;
    resp_ip_hdr->total_length = htons(sizeof(ip_header) + sizeof(icmp_header) + data_length);
    resp_ip_hdr->id = ip_request->id;
    resp_ip_hdr->fragment_offset = ip_request->fragment_offset;
    resp_ip_hdr->ttl = ip_request->ttl;
    resp_ip_hdr->protocol = IP_PROTO_ICMP;
    resp_ip_hdr->checksum = 0;
    resp_ip_hdr->src_ip = ip_request->dest_ip;
    resp_ip_hdr->dest_ip = ip_request->src_ip;
    resp_ip_hdr->checksum = calculate_checksum(resp_ip_hdr, sizeof(ip_header));

    // Fill ICMP header
    resp_icmp_hdr->type = ICMP_ECHO_REPLY;
    resp_icmp_hdr->code = 0;
    resp_icmp_hdr->checksum = 0;
    resp_icmp_hdr->id = icmp_request->id;
    resp_icmp_hdr->sequence = icmp_request->sequence;
    resp_icmp_hdr->checksum = calculate_checksum(resp_icmp_hdr, sizeof(icmp_header) + data_length);
    // Copy ICMP data
    memcpy(resp_data, (char *)(icmp_request + 1), data_length);

    // Send the ICMP reply
    write(tap_fd, response, sizeof(response));

}
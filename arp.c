#include <string.h>
#include <stdio.h>
#include "protocol.h"

#define MY_MAC {0x02, 0x00, 0x00, 0x00, 0x00, 0x01} // Example MAC address

void send_arp_reply(int tap_fd, ethernet_header *eth_hdr) {

    arp_header *arp_request = (arp_header *)(&eth_hdr[1]);

    if (ntohs(arp_request->opcode) != ARP_OP_REQUEST) {
        return; // Not an ARP request
    }

    char response[sizeof(ethernet_header) + sizeof(arp_header)];
    ethernet_header *resp_eth_hdr = (ethernet_header *)response;
    arp_header *resp_arp_hdr = (arp_header *)(response + sizeof(ethernet_header));
    
    // Fill Ethernet header
    memcpy(&resp_eth_hdr->dest_mac, &eth_hdr->src_mac, sizeof(mac_addr));
    mac_addr my_mac = MY_MAC;
    memcpy(&resp_eth_hdr->src_mac, &my_mac, sizeof(mac_addr));
    resp_eth_hdr->ethertype = htons(ETH_P_ARP);

    // Fill ARP header
    resp_arp_hdr->htype = arp_request->htype;
    resp_arp_hdr->ptype = arp_request->ptype;
    resp_arp_hdr->hlen = arp_request->hlen;
    resp_arp_hdr->plen = arp_request->plen;
    resp_arp_hdr->opcode = htons(ARP_OP_REPLY);
    memcpy(&resp_arp_hdr->sender_mac, &my_mac, sizeof(mac_addr));
    resp_arp_hdr->sender_ip = arp_request->target_ip;
    memcpy(&resp_arp_hdr->target_mac, &arp_request->sender_mac, sizeof(mac_addr));
    resp_arp_hdr->target_ip = arp_request->sender_ip;

    // Send the ARP reply
    write(tap_fd, response, sizeof(response));

}
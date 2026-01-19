#ifndef ARP_H
#define ARP_H

#include "protocol.h"

void send_arp_reply(int tap_fd, ethernet_header *eth_hdr);

#endif
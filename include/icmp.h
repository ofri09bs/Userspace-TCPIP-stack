#ifndef ICMP_H
#define ICMP_H

#include "protocol.h"
void send_icmp_reply(int tap_fd, ethernet_header *eth_hdr);

#endif // ICMP_H
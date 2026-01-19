#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <arpa/inet.h>
#include "protocol.h"
#include "arp.c"

#define BUFFER_SIZE 2048


int create_tap(char *device){
    struct ifreq ifr; // Interface request structure
    int fd, err;

    // Open the linux tuntap device file
    if ((fd = open("/dev/net/tun", O_RDWR)) < 0) {
        perror("Opening /dev/net/tun");
        return fd;
    }

    memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_flags = IFF_TAP | IFF_NO_PI; // TAP device without packet information

    if (*device) {
        strncpy(ifr.ifr_name, device, IFNAMSIZ);
    }

    // Create the TAP device using ioctl (it's a special system call)
    if ((err = ioctl(fd, TUNSETIFF, (void*) &ifr)) < 0) {
        perror("ioctl(TUNSETIFF)");
        close(fd);
        return err;
    }

    strcpy(device, ifr.ifr_name);
    return fd;
}

int main() {
    char tap_name[IFNAMSIZ];
    char buffer[BUFFER_SIZE];

    strcpy(tap_name, "tap0");
    int tap_fd = create_tap(tap_name);

    if (tap_fd < 0) {
        fprintf(stderr, "Error creating TAP device\n");
        return 1;
    }

    printf("TAP device %s created successfully\n", tap_name);
    printf("Listening for packets...\n");

    while(1)
    {
        int bytes_read = read(tap_fd, buffer, sizeof(buffer));
        if (bytes_read < 0) {
            perror("Reading from TAP device");
            close(tap_fd);
            return 1;
        }
        printf("Read %d bytes from %s\n", bytes_read, tap_name);

        ethernet_header *eth_hdr = (ethernet_header *)buffer;

        if (ntohs(eth_hdr->ethertype) == ETH_P_ARP) {
            printf("ARP packet received\n");
            // Process ARP packet
            arp_header *arp_hdr = (arp_header *)(buffer + sizeof(ethernet_header));
            if (ntohs(arp_hdr->opcode) == ARP_OP_REQUEST) {
                send_arp_reply(tap_fd, eth_hdr);
                printf("Sent ARP reply\n");
                       
            } else if (ntohs(arp_hdr->opcode) == ARP_OP_REPLY) {
                printf("Received ARP reply\n");
            }
        }
        if (ntohs(eth_hdr->ethertype) == ETH_P_IP) {
            
            ip_header *ip_hdr = (ip_header *)(buffer + sizeof(ethernet_header));
            int protocol = ip_hdr->protocol;
            printf("IP packet received, Protocol: %d\n", protocol);
            // Further processing for IP packets can be added here
        }
    }

    close(tap_fd);
    return 0;
}
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>

uint16_t calculate_checksum(void *vdata, size_t length) {
    char *data = (char *)vdata;
    uint32_t acc = 0;
    for (size_t i = 0; i + 1 < length; i += 2) {
        uint16_t word;
        memcpy(&word, data + i, 2);
        acc += ntohs(word);
    }
    if (length % 2 == 1) {
        uint16_t word = 0;
        memcpy(&word, data + length - 1, 1);
        acc += ntohs(word);
    }
    while (acc >> 16) {
        acc = (acc & 0xFFFF) + (acc >> 16);
    }
    return htons(~acc);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define HEXDUMP_CHARS_FOR_LINE 16

void hexdump(void *p, size_t len) {
    unsigned char *byte = p;
    size_t po = 0; // po = printed_offset

    for(size_t i = 0; i < len; i++) {
        printf("%02X ", byte[i]);
        if((i + 1) % HEXDUMP_CHARS_FOR_LINE == 0 ||  i == len - 1) {
            if(i == len - 1) {
                int padding = HEXDUMP_CHARS_FOR_LINE - (len % HEXDUMP_CHARS_FOR_LINE);
                padding %= HEXDUMP_CHARS_FOR_LINE;
                for(int j = 0; j < padding; j++) {
                    printf("~~ ");
                }
            }
            printf("\t");
            for(size_t j = po; j <= i; j++) {
                int c = isprint(byte[j]) ? byte[j] : '.';
                printf("%c", c);
            }
            printf("\n");
            po = i + 1;
        }
    }
}

#include "common.h"

/*
 * 16 items on a line
 */
void hexdump(const char* buffer, int size) {

    unsigned int index = 0;
    unsigned int line = 0;
    bool finished = false;

    printf("         0  1  2  3  4  5  6  7     8  9  A  B  C  D  E  F\n");
    while(!finished) {
        printf("0x%04X: ", index);
        for(line = 0; line < 8; line++) {
            if(line + index > (unsigned int)size) {
                finished = true;
                printf("   ");
            }
            else
                printf(" %02X", buffer[line+index]);
        }
        printf(" --");
        for(; line < 16; line++) {
            if(line + index > (unsigned int)size) {
                finished = true;
                printf("   ");
            }
            else
                printf(" %02X", buffer[line+index]);
        }
        printf("  ");
        for(line = 0; line < 16; line++) {
            if(line + index > (unsigned int)size) {
                finished = true;
                printf(" ");
            }
            else {
                int c = buffer[line+index];
                printf("%c", isprint(c)? c: '.');
            }
        }

        printf("\n");
        index += line;
    }
}


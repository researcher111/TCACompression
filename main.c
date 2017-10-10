#include <stdlib.h>
#include <stdio.h>

//28
static char *Table = 0;
static int tIndex = 0;

char * compressPacket(char data[], int TableSize, char *Table) {
    if (Table == 0) {
        Table = malloc(TableSize);
    }
    char *out = malloc(8);
    static int place = 0;
    int i;
    for (i = 0; i < 8; i++) {
        char index;
        for (index = 0; index < TableSize && Table[index] != data[i]; index++) {}
        if (index == TableSize) {
            printf("update ");
            tIndex++;
            tIndex %= TableSize;
            Table[tIndex] = data[i];
            int j;
            for (j = 3; j >= 0; j--) {
                char mask = 3;
                mask = mask << (j * 2);
                char partial = data[i] & mask;
                partial = partial >> (j * 2);
                int shift = (31 - place) % 4;
                char x = partial << shift * 2;
                char *byteout;
                int offset = place / 4;
                byteout = (out + offset);
                if (shift == 3)
                    *byteout = 0;
                *byteout = *byteout | x;
                place++;
            }
        } else {
            printf("compressed ");
            int shift = (31 - place) % 4;
            char x = index << shift * 2;
            char *byteout;
            int offset = place / 4;
            byteout = (out + offset);
            *byteout = *byteout | x;
            place++;
        }
    }
    return out;
}




int main(void)
{
    char data[8] = {2, 3, 12, 3, 17, 2, 3, 2};

    char *packet;
    packet = compressPacket(data, 3, Table);
    int i;
    printf("\n");
    for (i = 0; i < 8; i++) {
        int j;
        for (j = 7; j >= 0; j--) {
            char a = 1;
            a = a << j;
            char out = packet[i];
            out = out & a;
            out = out >> j;
            printf("%d ", out);
        }
        printf("\n");
    }
    return 0;
}

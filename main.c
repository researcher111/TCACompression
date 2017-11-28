#include <stdlib.h>
#include <stdio.h>

#define EMPTY 0

struct Packet {
    char data[19];
    int length;
};

//28
static char *Table = EMPTY;
static int tIndex = 0;

struct Packet compressPacket(char data[], int TableSize, char *Table) {
    if (Table == EMPTY) {
        Table = calloc(TableSize, 1);
    }
    char *out = calloc(16, 1);
    char boolean[2] = {0};
    char tr = 1; //boolean "true" value
    static int place = 0;
    int i;
    for (i = 0; i < 16; i++) {
        //Searching the Compression table for the matching, key value pair.
        //Continues until it has found a matching pair.
        char index = 0;
        while(index < TableSize && Table[index] != data[i]){
            index++;
        }
        //We have got the last index so the table does not contain a key, value pair for the packet.
        if (index == TableSize) {
            printf("update ");
            tIndex++;
            tIndex %= TableSize;
            Table[tIndex] = data[i];
            int j;
            for (j = 3; j >= 0; j--) {
                char mask = 0x03; // Mask 00000011
                mask = mask << (j * 2);
                char partial = data[i] & mask;
                partial = partial >> (j * 2);

                //TODO: update shifting.
                int shift = (63 - place) % 4;
                char x = partial << shift * 2;

                char *byteout;
                int offset = place / 4;
                byteout = (out + offset);
                *byteout = *byteout | x;
                place++;
            }
        } else {
            boolean[i / 8] = boolean[i / 8] | (tr << (7 - (i % 8)));
            printf("compressed ");
            //TODO: This about the math from calculating placement.
            int shift = (63 - place) % 4;
            char x = index << shift * 2;
            char *byteout;
            int offset = place / 4;
            byteout = (out + offset);
            *byteout = *byteout | x;
            place++;
        }
    }
    char count = ((place-1) / 4) + 2;
    struct Packet packet;
    packet.length = (count + 3);
    packet.data[0] = (count + 2);
    packet.data[1] = boolean[0];
    packet.data[2] = boolean[1];
    char k;
    for (k = 1; k <= count; k++) {
        packet.data[k + 2] = *(out + k - 1);
    }
    return packet;
}




int main(void)
{
    //char data[8] = {2, 3, 12, 3, 17, 2, 3, 2};
    char data[16] = {2, 3, 12, 3, 17, 2, 3, 2, 2, 3, 12, 3, 17, 2, 3, 2};

    struct Packet packet;
    packet = compressPacket(data, 5, Table);
    printf("\n");
    int i;
    for (i = 0; i < packet.length; i++) {
        int j;
        for (j = 7; j >= 0; j--) {
            char a = 1;
            a = a << j;
            char out = packet.data[i];
            out = out & a;
            out = out >> j;
            out = out & 1;
            printf("%d ", out);
        }
        printf("\n");
    }
    return 0;
}

#include <stdlib.h>
#include <stdio.h>

#define EMPTY 0


//28
static char *Table = EMPTY;
static int tIndex = 0;

char * compressPacket(char data[], int TableSize, char *Table) {
    if (Table == EMPTY) {
        Table = calloc(TableSize);
    }
    char *out = calloc(8);
    static int place = 0;
    int i;
    for (i = 0; i < 8; i++) {
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
                int shift = (31 - place) % 4;
                char x = partial << shift * 2;
                
                char *byteout;
                int offset = place / 4;
                byteout = (out + offset);
                *byteout = *byteout | x;
                place++;
            }
        } else {
            printf("compressed ");
            //TODO: This about the math from calculating placement. 
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

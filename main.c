
#include <msp430.h>
#define RedLed BIT0
#define GreenLed BIT7

unsigned int RXByteCtr = 0;
void transmit(const char *str, int size);




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


/*
 * main.c
 */
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    /*  BCSCTL1 = CALBC1_1MHZ;//Adjust the clock
     DCOCTL = CALDCO_1MHZ;*/
    P1DIR = RedLed; //Make P1.0 an output so we can use the red LED
    P4DIR = GreenLed; //Make P4.7 an output so we can use the red LED
    P1OUT &= ~RedLed;  //Clear the red LED
    P4OUT &= ~GreenLed;  //Clear the green LED

    P3SEL = BIT3 + BIT4;                        // P3.3,4 = USCI_A0 TXD/RXD
    UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 6;                              // 1MHz 9600 (see User's Guide)
    UCA0BR1 = 0;                              // 1MHz 9600
    UCA0MCTL = UCBRS_0 + UCBRF_13 + UCOS16;   // Modln UCBRSx=0, UCBRFx=0,
// over sampling
    UCA0CTL1 &= ~UCSWRST;                   // **Initialize USCI state machine**
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
    _enable_interrupts();

    char data[16] = {2, 3, 12, 10, 3 , 10, 3, 2, 2, 3, 12, 3, 10, 2, 3, 2};
    struct Packet packet;

    while (1) {
        packet = compressPacket(data, 5, Table);

        transmit(packet.data, packet.length);

        __delay_cycles(2000000);

        RXByteCtr = 0;

    }                         //Reset Receive Byte counter
}

//USCI A receiver interrupt
// The stuff immediately below is to make it compatible with GCC, TI or IAR
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{   //Check if the UCA0RXBUF is different from 0x0A
//(Enter key from keyboard)
    if(UCA0RXBUF != 0x0A) input[RXByteCtr++] = UCA0RXBUF;
//If it is, load received character
//to current input string element
    else {cnt = 1;
        //If it is not, set cnt
        input[RXByteCtr-1] = 0;
    }   //Add null character at the end of input string (on the /r)
}
void transmit(const char *str, int size) {
    int count = size;
    while (count != 0) { //Do this during current element is not
        //equal to null character
        while (!(UCTXIFG & UCA0IFG));
        //Ensure that transmit interrupt flag is set
        UCA0TXBUF = *str++;
        //Load UCA0TXBUF with current string element
        count--;
    }       //then go to the next element
}

int max(int a, int b) {
    if (a > b)
        return a;
    else
        return b;
} // Find the max between two numbers.



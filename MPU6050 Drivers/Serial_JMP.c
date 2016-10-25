#include <msp430F5529.h>
#include "Serial_JMP.h"

/*
 * Serial_JMP.c
 *
 *  Created on: Oct 25, 2016
 *  Author: Jose A. Montes Perez
 *
 *
 *  This file contains implementations
 *  of the I2C and UART protocols for
 *  the msp430f5529
 */


////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////UART functionality




/*
 * Sets up the baud generator for a specific frequency
 * Parameters-
 * 	baudRate - defines the baud rate for communication
 * 		valid frequencies are 9600 and its multiples up to
 * 		115200. Suggested low frequency
 *
 *
 */

void uart_setup(long baudRate){
	P3SEL = BIT3+BIT4;                        // P3.4,5 = USCI_A0 TXD/RXD
	UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
	UCA0CTL1 |= UCSSEL_2;                     // CLK = SUBMAINCLK
	switch(baudRate){
	case 9600:
		UCA0BR0 = 109;                           // 1.048MHz/9600=109 (see User's Guide)
		UCA0BR1 = 0x00;                           //
		UCA0MCTL |= UCBRS_2+UCBRF_0;               // Modulation UCBRSx=3, UCBRFx=0
		break;
	case 19200:
		UCA0BR0 = 52;
		UCA0BR1 = 0x00;
		UCA0MCTL |= UCBRS_0 +UCBRF_0;               // Modulation UCBRSx=0, UCBRFx=0
		break;
	case 38400: break;
		UCA0BR0 = 26;
		UCA0BR1 = 0;
		UCA0MCTL |= UCBRS_0 +UCBRF_0;               // Modulation UCBRSx=0, UCBRFx=0
		break;
	case 57600: break;
		UCA0BR0 = 17;
		UCA0BR1 = 0;
		UCBA0MCTL = UCBRS_3 + UCBRS_0;
		break;
	case 115200: break;
	default:
		P4DIR |= BIT7;
		while(1){
			P4OUT ^= BIT7;
			__delay_cycles(100000);
		}
	}
	UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
	UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
}

void transmit(char byte){
}


















void main(void){
	return;
}

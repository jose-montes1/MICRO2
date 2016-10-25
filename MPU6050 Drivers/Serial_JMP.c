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
	//UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
}

void transmit_byte(char byte){
	while(!(UCA0IFG));
	UCA0TXBUFF = byte;
}


void serial_print(char *string){
	while(*string){
		transmit(*string);
		string++;
	}
}



//UART Interrupt Vector
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void){
	if(UCA0IV == 0x02){
		buff = UCA0RXBUF;
		//LPM3_EXIT;
	}
}
///// END OF UART
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
///// I2C Functionality
unsigned char TXData;
unsigned char TXByteCtr;
int Rx;
void transmit(char byte){
	Rx = 0;
	TXData = byte;
    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
    UCB0CTL1 |= UCTR + UCTXSTT;             // I2C TX, start condition
    __bis_SR_register(LPM0_bits + GIE);        // Enter LPM0 w/ interrupts
}
void receive(void){
    Rx = 1;
	__bic_SR_register(GIE);
	while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
	UCB0CTL1 &= ~UCTR ;                     // Clear UCTR
	UCB0CTL1 |= UCTXSTT;                    // I2C start condition
	while (UCB0CTL1 & UCTXSTT);             // Start condition sent?
	UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
	__bis_SR_register(LPM0_bits + GIE);     // Enter LPM0 w/ interrupts
}

void i2cSetup(int baudRate){

  P3SEL |= 0x03;                            // Assign I2C pins to USCI_B0
  UCB0CTL1 |= UCSWRST;                      // Enable SW reset
  UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
  UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
  if(baudRate == 10000){
	UCB0BR0 = 105;                             // fSCL = SMCLK/12 = ~10kHz
	UCB0BR1 = 0;
  
  }
  UCB0I2CSA = 0x68;                         // Slave Address is 048h
  UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
  UCB0IE |= UCRXIE + UCTXIE;

}
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void){
  if(Rx){
	  buff = UCB0RXBUF;
	  UCB0IFG &= ~UCRXIFG;
	  __bic_SR_register_on_exit(LPM0_bits);
  }							 // Vector 12: TXIFG
  else{
	if (TXByteCtr){                          // Check TX byte counter

	  UCB0TXBUF = TXData;                   // Load TX buffer
	  TXByteCtr--;                          // Decrement TX byte counter
	}
	else{
	  UCB0CTL1 |= UCTXSTP;                  // I2C stop condition
	  UCB0IFG &= ~UCTXIFG;                  // Clear USCI_B0 TX int flag
	  __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0
	}
  }
}




void main(void){
	return;
}

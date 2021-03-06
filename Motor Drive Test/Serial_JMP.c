#include <msp430F5529.h>
#include "Serial_JMP.h"
#include "General_JMP.h"
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
/*\**********************************************************************************************************
 * Function Description
 *    Sets up the baud generator for a specific frequency
 * Parameters-
 * 	baudRate - defines the baud rate for communication
 * 		valid frequencies are 9600 and its multiples up to
 * 		115200. Suggested low frequency
 *\**********************************************************************************************************/

void UART_setup(unsigned long baudRate){
	P3SEL = BIT3+BIT4;                        // P3.4,5 = USCI_A0 TXD/RXD
	P3REN = BIT3;							// Set up resistor for BT module
	P3OUT = BIT3;							// Make it pull up
	UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
	UCA0CTL1 |= UCSSEL_2;                     // CLK = SUBMAINCLK
	switch(baudRate){
	case 9600:
		UCA0BR0 = 109;                           // 1.048MHz/9600=109 (see User's Guide)
		UCA0BR1 = 0x00;                          //
		UCA0MCTL |= UCBRS_2+UCBRF_0;             // Modulation UCBRSx=3, UCBRFx=0
		break;
	case 19200:
		UCA0BR0 = 52;
		UCA0BR1 = 0x00;
		UCA0MCTL |= UCBRS_0 +UCBRF_0;            // Modulation UCBRSx=0, UCBRFx=0
		break;
	case 38400:
		UCA0BR0 = 26;
		UCA0BR1 = 0;
		UCA0MCTL |= UCBRS_0 +UCBRF_0;            // Modulation UCBRSx=0, UCBRFx=0
		break;
	case 57600:
		UCA0BR0 = 17;
		UCA0BR1 = 0;
		UCA0MCTL = UCBRS_3 + UCBRS_0;
		break;
	case 115200:
	default:									//Catch incorrect error
		P4DIR |= BIT7;							// Set on board LED on
		while(1){								// Error trap infinite while loop
			P4OUT ^= BIT7;						// Toggle led
			delay(500);				// Delay
		}
	}
	UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**

}
/*\**********************************************************************************************************
 *Function Description
 * 	Transmits one byte over the Serial UART line
 *Parameters
 *	Byte - information to be sent
 *\**********************************************************************************************************/
void UART_transmit_byte(unsigned char byte){
	while(!(UCA0IFG & UCTXIFG));
	UCA0TXBUF = byte;
}


/*\**********************************************************************************************************
 *Function Description
 * 	Receives one Byte of information from the UART serial line
 *Parameters
 *	Dest - destination were the byte will be stored
 *\**********************************************************************************************************/
char uartBuff;
void UART_receive_byte(char *dest){
	UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
	LPM0;									  // Wait for interrupt
	*dest = uartBuff;						  // Receive from global variable
	UCA0IE &= ~UCRXIE;                         // Disable USCI_A0 RX interrupt
}

/*\**********************************************************************************************************
 *Function Description
 * 	Prints out a stream of data from the uart line
 *Parameters
 *  String - Character array to be printed
 *\**********************************************************************************************************/
void UART_print(char *string){
	while(*string){							// While there is more content
		UART_transmit_byte(*string);				// Send one byte
		string++;							// Load next byte
	}
}

/*\**********************************************************************************************************
 *Function Description
 * 	Prints out a stream of data from two data collections
 *Parameters
 *  String1 - Character array to be printed
 *  String2 - Character array to be printed
 *\**********************************************************************************************************/
void UART_print_status(unsigned char *string1,unsigned char *string2){
	while(*string1){					    // While there is more content
		UART_transmit_byte(*string1);			// Send one byte
		string1++;							// Load next byte
	}
	while(*string2){						// While there is more content
		UART_transmit_byte(*string2);			// Send one byte
		string2++;							// Load next byte
	}
}

void UART_print_value(char *string, int value){
	char buffer[16];
	itoa(value, buffer);
	unsigned char *intArr = (unsigned char *)buffer;
	while(*string){
		UART_transmit_byte(*string);
		string++;
	}
	while(*intArr){
			UART_transmit_byte(*intArr);
			intArr++;
		}
}


//UART Interrupt Vector
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void){
	if(UCA0IV == 0x02){						// Receive interrupt
		uartBuff = UCA0RXBUF;				// Export to global variable
		LPM0_EXIT;							// Exit low power mode
	}
}
///// END OF UART
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
///// I2C Functionality
unsigned char txAddress;
unsigned char txData;
int *rxDest;
unsigned char rxByteCtr;
unsigned char txByteCtr;
int Rx;
/*\**********************************************************************************************************
 *Function Description
 *	 Writes a stream of data to an I2C bus
 *Parameters
 * 	 Slave Address - Address of the slave to receive data
 * 	 Register Address - Internal register of the slave to access
 * 	 Data pointer - Byte or collection of consecutive bytes to write to the slave
 * 	 Amount - Amount of bytes to send
 *\\**********************************************************************************************************/

void I2C_write(unsigned char slaveAddress, unsigned char registerAddress, unsigned char data){
	Rx = 0;									// Set to transmit
	txByteCtr = 2;						    // Set global variable
	txAddress = registerAddress;				// Set global variable
	txData = data;
	UCB0I2CSA = slaveAddress;               // Slave Address is 048h
    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
    UCB0CTL1 |= UCTR + UCTXSTT;             // I2C TX, start condition
    __bis_SR_register(LPM0_bits + GIE);     // Enter LPM0 w/ interrupts
}

/*\***********************************************************************************************************
 *Function Description
 * 	Reads a stream of data from an I2C Slave
 *Parameter description
 *  Slave Address - Address of the slave to access
 * 	Register Address - Internal register of the slave to access
 * 	Read Destination - pointer to the variable in memory to store
 * 	Length - amount of bytes to dump onto the read destination
 *\***********************************************************************************************************/


void I2C_read(unsigned char slaveAddress,unsigned char registerAddress, int *readDestination, int length){
    I2C_write(slaveAddress, registerAddress, 0xFF);
	__bic_SR_register(GIE);
	Rx = 1;									// Set to receive
	rxByteCtr = length;						// Set global variable
	rxDest = readDestination;
	while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
	UCB0CTL1 &= ~UCTR ;                     // Clear UCTR
	UCB0CTL1 |= UCTXSTT;                    // I2C start condition
	if(rxByteCtr == 1){
		while (UCB0CTL1 & UCTXSTT);         // Start condition sent?
		UCB0CTL1 |= UCTXSTP;                // I2C stop condition
	}
	__bis_SR_register(LPM0_bits + GIE);     // Enter LPM0 w/ interrupts
}
/*\***********************************************************************************************************
 *Function Description
 * 	Bootstrap for the I2C serial bus on UCB0
 *Parameter description
 *  Baud rate - defines the synchronous clock rate to be utilized
 *
 *\************************************************************************************************************/
void I2C_setup(unsigned long baudRate){

  P3SEL |= 0x03;                            // Assign I2C pins to USCI_B0
  UCB0CTL1 |= UCSWRST;                      // Enable SW reset
  UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
  UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
  if(baudRate == 10000){
	UCB0BR0 = 105;                          // fSCL = SMCLK/105 = ~10kHz
	UCB0BR1 = 0;
  }if(baudRate == 100000){
	UCB0BR0 = 105;                          // fSCL = SMCLK/12 = ~100kHz
  	UCB0BR1 = 0;
  }if(baudRate == 400000){
	  UCB0BR0 = 3;
	  UCB0BR1 = 0;
  }
  UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
  UCB0IE |= UCRXIE + UCTXIE;                // Enable interrupts

}
/*\***********************************************************************************************************
 *Function Description
 * 	I2C Interrupt function to handle data amount
 *Parameter description
 *  Baud rate - defines the synchronous clock rate to be utilized
 *
 *\************************************************************************************************************/
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void){
  if(Rx){									// Check if you are receiving
	  rxByteCtr--;							// Decrease Rx counter
	  if(rxByteCtr){						// More than 1 value left
		  *rxDest = UCB0RXBUF;				// Receive into memory
		  if(rxByteCtr == 1){				// One byte left
			  UCB0CTL1 |= UCTXSTP;			// Initiate stop after next byte read
		  }
	  }else{								// Last byte to read
		  *rxDest = UCB0RXBUF;				// Allocate in memory
		  UCB0IFG &= ~UCRXIFG;				// Clear any mistaken flag
		  __bic_SR_register_on_exit(LPM0_bits);	//Exit low power mode
	  }
  }
  else{
	switch(txByteCtr){                          // Check TX byte counter
		case 2:
			UCB0TXBUF = txAddress;              // Load TX buffer
			txByteCtr--;						// Decrement TX byte counter
			break;
		case 1:
			if(!(txData == 0xFF)){
				UCB0TXBUF = txData;
			}
			txByteCtr--;
			break;
		case 0:
		    UCB0CTL1 |= UCTXSTP;                  // I2C stop condition
		    UCB0IFG &= ~UCTXIFG;                  // Clear USCI_B0 TX int flag
		   __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0
		   break;
		default: break;
	}
  }
}

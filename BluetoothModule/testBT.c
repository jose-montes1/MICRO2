#include <msp430.h>
#include "SerialBluetooth.h"
/*
 * text.c
 *
 *  Created on: Oct 26, 2016
 *      Author: enriquetorres
 *
 */
void main(void){
	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
	P1DIR |= BIT0;
	initBTModule();
	sendString("Hello mundo!!");
	sendByte('?');
	__bis_SR_register(GIE);
	getConnectionStatus();
	sendByte(BTConnected+0x30);
	LPM3;
}

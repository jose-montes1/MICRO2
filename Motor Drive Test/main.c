#include <msp430.h> 
#include "General_JMP.h"
#include "Serial_JMP.h"

/*
 * This file is to test the motor configuration of the
 * Wave x
 *
 * Wiring
 * Motors---------------------------------------------
 * P1.2 -> blue
 * P1.3 -> purple
 * P1.4 -> white
 * P1.5 -> grey
 *
 * UART-----------------------------------------------
 * P3.4(RX) -> TX
 * P3.3(TX) -> RX
 *
 *
 */
void turn_right(){
	P1OUT &= ~(BIT2 + BIT3 + BIT4 + BIT5);				//Clear last input
	P1OUT |= (BIT2 + BIT4);								//Set the relays to turn counterclockwise
}

void turn_left(){
	P1OUT &= ~(BIT2 + BIT3 + BIT4 + BIT5);				//Clear last input
	P1OUT |= (BIT3 + BIT5);								//Set the relay to turn clockwise
}
void go_forward(){
	P1OUT &= ~(BIT2 + BIT3 + BIT4 + BIT5);				//Clear the last iput
	P1OUT |= (BIT3 + BIT4);
}
void go_backwards(){
	P1OUT &= ~(BIT2 + BIT3 + BIT4 + BIT5);
	P1OUT |= (BIT2 + BIT5);
}
void off(){
	P1OUT &= ~(BIT2 + BIT3 +BIT4 + BIT5);
}
void BT_timeout_setup(int miliseconds){
	unsigned long operations = (32768*miliseconds)/1000;
	TA1CTL |= TBCLR; 				// Clear Timer content
	TA1CCR0 = operations;			// Set the timer period
	TA1CTL |= TBSSEL_1;				// Use aclk
	TA1CCTL0 |= CCIE;				// Enable Interrupt
}

int received_command;
void BT_restart_timeout(){
	TA1R = 0;						// Reset Counter
	TA1CTL |= MC_1;					// Use up mode
	received_command = 1;
}
void BT_stop_timeout(){
	TA1CTL &= ~MC_1;				//Stop timer
}


#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void){
	TA1CTL &= ~MC_1;				//Stop timer
	TA1CTL |= MC_0;					//Make sure its stopped
	received_command = 0;
	LPM0_EXIT;
}



int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	P1DIR |= BIT2+BIT3+BIT4+BIT5;
	char command;
	UART_setup(9600);
	BT_timeout_setup(1000);
	_BIS_SR(GIE);

	while(1){
		BT_restart_timeout();
		UART_receive_byte(&command);
		BT_stop_timeout();
	//	if(received_command == 0){
	//		command = 'n';
	//	}
		if(command == 'n'){
			off();
		}else if(command == 'a'){
			turn_right();
		}else if(command == 'd'){
			turn_left();
		}else if(command == 'w'){
			go_forward();
		}else if(command == 's'){
			go_backwards();
		}else if(command == 'o'){
			off();
		}
	}
}

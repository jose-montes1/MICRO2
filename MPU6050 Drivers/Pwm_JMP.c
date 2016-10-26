#include <msp430f5529.h>
#include "Pwm_JMP.h"
#include "Serial_JMP.h"
/*
 * Pwm_JMP.c
 *
 *  Created on: Oct 26, 2016
 *      Author: jose.montes1
 *
 *  This file is utilized as a generic PWM
 *  generator with added servo functionality
 *
 */


void pwm_setup(){
	P1DIR |= BIT5;								// Set as ouput
	P1SEL |= BIT5;								// Select timer module


	TA0CCR0 = ServoMS;

	TA0CCTL4 = OUTMOD_7;                        // CCR4 reset/set
	TA0CCR4 = 0;                                 // CCR1 PWM duty cycle

	TA0CTL = TASSEL_1 + MC_1 + TACLR;           // ACLK, up mode, clear T


}
void pwm_write_speed(int speed){
	int microseconds;
	if(speed < 100 & speed > -100){
		microseconds = DEADBAND + speed*RANGE;
		pwm_write_microseconds(microseconds);

	}else{
		UART_print("Wrong number");
	}
}

void pwm_write_microseconds(int microseconds){
	float dutyCycle = microseconds*100/20000;
	pwm_write_duty_cycle(dutyCycle);
}

void pwm_write_duty_cycle(float dutyCycle){

	int value = ((float) (dutyCycle*50Hz))/100;	// Calculate the appropriate register value
	TA0R = 0;
	TA0CCR4 = value;							// Send register value

}

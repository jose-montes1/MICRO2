#include <msp430.h> 
#include "Serial_JMP.h"
#include "Pwm_JMP.h"


/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
    UART_setup(9600);
    pwm_setup();       			//Sets port 1.5 to output pwm

    char command = 0;
    while(command != 'a'){
    	UART_print("hello world");
    	UART_receive_byte(&command);
    }


    int speed = 0;
    pwm_write_speed(speed);
    while(1){
    	UART_receive_byte(&command);
    	switch (command) {
    	case 'a':
    		speed += 10;
    		break;
    	case 'b':
    		speed -= 10;
    		break;
    	default:
    		break;
    	}
    	pwm_write_speed(speed);
    	command = 0;
    }
}

#include <msp430.h> 
#include "Serial_JMP.h"
#include "MPU6050.h"

/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	I2C_setup(10000);
	mpu_test_connection();
	return 0;
}

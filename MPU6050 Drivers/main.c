#include <msp430.h> 
#include "Serial_JMP.h"
#include "MPU6050.h"
#include "General_JMP.h"

/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    _BIS_SR(GIE);
    UART_setup(9600);
    UART_print("starting");
	I2C_setup(10000);
	UART_print("\fI2C setup");
	mpu_test_connection();
	UART_print_value("\n\rThis is a test: ", 30);
	mpu_setup();
	mpu_calibrate_gyro();

	__no_operation();

	while(1){
		mpu_read_gyro_rate();
		mpu_read_accel();
		mpu_print();
		delay(200);
	}
}

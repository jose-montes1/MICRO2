/*
 * Serial_JMP.h
 *
 *  Created on: Oct 25, 2016
 *      Author: Rathk
 */

#ifndef SERIAL_JMP_H_
#define SERIAL_JMP_H_

#define SUCCESS "\n\rProcess successful: "
#define FAIL "\n\rProcess failed: "


void UART_setup(unsigned long baudRate);
void UART_transmit_byte(unsigned char byte);
void UART_receive_byte(char *dest);
void UART_print(char *string);
void UART_print_status(unsigned char *string1,unsigned char *string2);
void UART_print_value(char *string, int value);
__interrupt void USCI_A0_ISR(void);

void I2C_setup(unsigned long baudRate);
void I2C_write(unsigned char slaveAddress, unsigned char registerAddress, unsigned char data);
void I2C_read(unsigned char slaveAddress, unsigned char registerAddress,int *readDestination, int length);
__interrupt void USCI_B0_ISR(void);

void BT_timeout_setup(int miliseconds);
void BT_start_timeout();
void BT_stop_timeout();
__interrupt void TIMER1_A0_ISR(void);

#endif /* SERIAL_JMP_H_ */

#include "MPU6050.h"
#include "Serial_JMP.h"
#include <math.h>
/*
 * MPU6500.c
 *
 *  Created on: Oct 24, 2016
 *  Author: Jose A. Montes
 *
 *  Based on the code found on:
 *  http://www.botched.co.uk/pic-tutorials/mpu6050-setup-data-aquisition/
 */

/////////////////////////////////////////////////////////////
/// Constant decalrations

//Gyroscope values
int gyroXout;
int gyroYout;
int gyroZout;

//Gyroscope calibration
int gyroXoutOffset;
int gyroYoutOffset;
int gyroZoutOffset;

//Gyroscope Rates
int gyroXout_H, gyroXout_L;
int gyroYout_H, gyroYout_L;
int gyroZout_H, gyroZout_L;

//Accelerometer values
int accelXout;
int accelYout;
int accelZout;

//Accelerometer angles
int accelXangle;
int accelYangle;


//Accelerometer intermediate values
int accelXout_H, accelXout_L;
int accelYout_H, accelYout_L;
int accelZout_H, accelZout_L;



/***********************************************************
 *
 * Test connection to MPU6500
 * Utilized to verify that you are properly connected
 * -Checks RA to the mpu slave addres, value should be
 * equal to 0x68
 *
 *********************************************************/

void mpu_test_connection(){
	int address;
	I2C_read(MPU6050_ADDRESS, MPU6050_RA_WHO_AM_I, &address, 1);
	if(address == MPU6050_ADDRESS){
		UART_print_status(SUCCESS, "MPU6500 Verified");
	}else{
		UART_print_status(FAIL, "MPU6500 Not verified");
		while(1);
	}
}

/************************************************
 *
 * Configures the internal registers of the MPU 6500
 * Utilized to initialize the mpu6050
 *
 ************************************************/
void mpu_setup(void){
	//Set sample rate to 1k
	I2C_write(MPU6050_ADDRESS,  MPU6050_RA_SMPLRT_DIV, 0x07);
	//Disable external FSYNC
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_CONFIG, 0x00);
	//Disable gyro self test, scale of 500 deg/s
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_GYRO_CONFIG, 0x08);
	//Disable acc self test, scale of -+2G
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_ACCEL_CONFIG, 0x00);
	//Free fall threshold of |0mg|
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_FF_THR, 0x00);
	//Free fall duration litmit of 0mg
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_FF_DUR, 0x00);
	//Motion threshold of 0mg
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_MOT_THR, 0x00);
	//Motion duration of 0s
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_MOT_DUR, 0x00);
	//Zero motion threshold
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_ZRMOT_THR, 0x00);
	//Zero motion duration
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_ZRMOT_DUR, 0x00);
	//Disable output to fifo buffer
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_FIFO_EN, 0x00);

	//AUX I2C setup - external I2C connection
	//Setup to no external connector
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_I2C_MST_CTRL, 0x00);
	//Initialize slave addresses to 0
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV0_ADDR, 0x00);
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV0_REG, 0x00);
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV0_CTRL, 0x00);
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV1_ADDR, 0x00);
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV1_REG, 0x00);
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV1_CTRL, 0x00);
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV2_ADDR, 0x00);
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV2_REG, 0x00);
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV2_CTRL, 0x00);
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV3_ADDR, 0x00);
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV3_REG, 0x00);
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV3_CTRL, 0x00);
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV4_ADDR, 0x00);
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV4_REG, 0x00);
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV4_DO, 0x00);
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV4_CTRL, 0x00);
	I2C_write(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV4_DI, 0x00);

	//Setup INT pin and AUX I2C pass through
    I2C_write(MPU6050_ADDRESS, MPU6050_RA_INT_PIN_CFG, 0x00);
    //Enable data ready interrupt
    I2C_write(MPU6050_ADDRESS, MPU6050_RA_INT_ENABLE, 0x00);
    //Slave out, dont care
    I2C_write(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV0_DO, 0x00);
    I2C_write(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV1_DO, 0x00);
    I2C_write(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV2_DO, 0x00);
    I2C_write(MPU6050_ADDRESS, MPU6050_RA_I2C_SLV3_DO, 0x00);
    //More slave config
    I2C_write(MPU6050_ADDRESS, MPU6050_RA_I2C_MST_DELAY_CTRL, 0x00);
    //Reset sensor signal paths
    I2C_write(MPU6050_ADDRESS, MPU6050_RA_SIGNAL_PATH_RESET, 0x00);
    //Motion detection control
    I2C_write(MPU6050_ADDRESS, MPU6050_RA_MOT_DETECT_CTRL, 0x00);
	//Disables FIFO, AUX I2C, FIFO and I2C reset bits to 0
    I2C_write(MPU6050_ADDRESS, MPU6050_RA_USER_CTRL, 0x00);
	//Sets clock source to gyro reference w/ PLL
    I2C_write(MPU6050_ADDRESS, MPU6050_RA_PWR_MGMT_1, 0b00000010);
	//Controls frequency of wakeups in accel low power mode plus the sensor standby modes
    I2C_write(MPU6050_ADDRESS, MPU6050_RA_PWR_MGMT_2, 0x00);
	//MPU6050_RA_BANK_SEL            //Not in datasheet
	//MPU6050_RA_MEM_START_ADDR        //Not in datasheet
	//MPU6050_RA_MEM_R_W            //Not in datasheet
	//MPU6050_RA_DMP_CFG_1            //Not in datasheet
	//MPU6050_RA_DMP_CFG_2            //Not in datasheet
	//MPU6050_RA_FIFO_COUNTH        //Read-only
	//MPU6050_RA_FIFO_COUNTL        //Read-only
	//Data transfer to and from the FIFO buffer
    I2C_write(MPU6050_ADDRESS, MPU6050_RA_FIFO_R_W, 0x00);
	//MPU6050_RA_WHO_AM_I             //Read-only, I2C address

    UART_print_status(SUCCESS, "MPU6050 Setup Complete");
}

/*********************************************************************
 * Calibration process
 * Takes the avg of 1000 values of the gyro as the base value
 * Can recalibrate during setup
 *
 */


void mpu_calibrate_gyro(){

	int gyroXOffsetSum = 0;
	int gyroYOffsetSum = 0;
	int gyroZOffsetSum = 0;


	int iterations = 0;
	for(iterations = 0; iterations < 1000; iterations++){
		I2C_read(MPU6050_ADDRESS, MPU6050_RA_GYRO_XOUT_H, &gyroXout_H, 1);
		I2C_read(MPU6050_ADDRESS, MPU6050_RA_GYRO_XOUT_L, &gyroXout_L, 1);
		I2C_read(MPU6050_ADDRESS, MPU6050_RA_GYRO_YOUT_H, &gyroYout_H, 1);
		I2C_read(MPU6050_ADDRESS, MPU6050_RA_GYRO_YOUT_L, &gyroYout_L, 1);
		I2C_read(MPU6050_ADDRESS, MPU6050_RA_GYRO_ZOUT_H, &gyroZout_H, 1);
		I2C_read(MPU6050_ADDRESS, MPU6050_RA_GYRO_ZOUT_L, &gyroZout_L, 1);

		gyroXOffsetSum += ((gyroXout_H << 8) |gyroXout_L);
		gyroYOffsetSum += ((gyroYout_H << 8) |gyroYout_L);
		gyroZOffsetSum += ((gyroZout_H << 8) |gyroZout_L);
		delay(1);

	}
	gyroXoutOffset = gyroXOffsetSum/1000;
	gyroYoutOffset = gyroYOffsetSum/1000;
	gyroZoutOffset = gyroZOffsetSum/1000;

	UART_print_value("\n\rCalibration done, X offset: ", gyroXoutOffset);
	UART_print_value("\n\rCalibration done, Y offset: ", gyroYoutOffset);
	UART_print_value("\n\rCalibration done, Z offset: ", gyroZoutOffset);

}
/**********************************************************************************
 * Reads raw accelerometer values
 *
 *
 */

void mpu_read_accel(){

	I2C_read(MPU6050_ADDRESS, MPU6050_RA_ACCEL_XOUT_H, &accelXout_H, 1);
	I2C_read(MPU6050_ADDRESS, MPU6050_RA_ACCEL_XOUT_H, &accelXout_L, 1);
	I2C_read(MPU6050_ADDRESS, MPU6050_RA_ACCEL_XOUT_H, &accelYout_H, 1);
	I2C_read(MPU6050_ADDRESS, MPU6050_RA_ACCEL_XOUT_H, &accelYout_L, 1);
	I2C_read(MPU6050_ADDRESS, MPU6050_RA_ACCEL_XOUT_H, &accelZout_H, 1);
	I2C_read(MPU6050_ADDRESS, MPU6050_RA_ACCEL_XOUT_H, &accelZout_L, 1);

	accelXout = ((accelXout_H << 8) | accelXout_L);
	accelYout = ((accelYout_H << 8) | accelYout_L);
	accelZout = ((accelZout_H << 8) | accelZout_L);

}

/*******************************************************************************
 *
 * Processes the obtained acceleration to euler angles
 *
 ********************************************************************************/


void mpu_calc_accel_angles(){
	accelXangle = 57.295*atan((float)accelYout/ sqrt(pow((float)accelZout,2)+pow((float)accelXout,2)));
	accelYangle = 57.295*atan((float)-accelXout/ sqrt(pow((float) accelZout,2)+pow((float)accelYout,2)));
}

/**********************************************************************************
 *
 * Reads the accelerometer rates in angles/seconds.
 *
 *********************************************************************************/

void mpu_read_gyro_rate(){

	I2C_read(MPU6050_ADDRESS, MPU6050_RA_GYRO_XOUT_H, &gyroXout_H, 1);
	I2C_read(MPU6050_ADDRESS, MPU6050_RA_GYRO_XOUT_L, &gyroXout_L, 1);
	I2C_read(MPU6050_ADDRESS, MPU6050_RA_GYRO_YOUT_H, &gyroYout_H, 1);
	I2C_read(MPU6050_ADDRESS, MPU6050_RA_GYRO_YOUT_L, &gyroYout_L, 1);
	I2C_read(MPU6050_ADDRESS, MPU6050_RA_GYRO_ZOUT_H, &gyroZout_H, 1);
	I2C_read(MPU6050_ADDRESS, MPU6050_RA_GYRO_ZOUT_L, &gyroZout_L, 1);

	gyroXout = ((gyroXout_H << 8) | gyroXout_L) - gyroXoutOffset;
	gyroYout = ((gyroYout_H << 8) | gyroYout_L) - gyroYoutOffset;
	gyroZout = ((gyroZout_H << 8) | gyroZout_L) - gyroZoutOffset;

	gyroXout = (float)gyroXout/gyroXsensitivity;
	gyroYout = (float)gyroYout/gyroYsensitivity;
	gyroZout = (float)gyroZout/gyroZsensitivity;
}

void mpu_print(){

	UART_print_value("\fX accel: ", accelXout);
	UART_print_value("\n\rY accel: ", accelYout);
	UART_print_value("\n\rY accel: ", accelZout);
	UART_print_value("\n\rX gyro: ", gyroXout);
	UART_print_value("\n\rY gyro: ", gyroYout);
	UART_print_value("\n\rZ gyro: ", gyroZout);

}


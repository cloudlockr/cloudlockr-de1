/*
 * MPU9250.h
 *
 */

/*
 * Description
 * This module is an adoption of MPU9250 sensor device driver by ????.
 *
 * Note:
 * SPI0 frequency Will stick to 1 MHz. Runtime frequency variants are not supported.
 *
 */


#ifndef MPU9250_H
#define MPU9250_H

#include "constants.h"

/*------------------- API Function -------------------------------------------*/

bool MPU9250_Begin( void );
bool MPU9250_EnableDrdyInt( void );
bool MPU9250_DisableDrdyInt( void );
bool MPU9250_ConfigSrd( const uint8 srd );
bool MPU9250_ConfigAccelRange( const uint8 range );
bool MPU9250_ConfigGyroRange( const uint8 range );
bool MPU9250_ConfigDlpf(const uint8 dlpf);
//void MPU9250_DrdyCallback( uint8 int_pin, void (*function)() );
bool MPU9250_Read( void );

//void  MPU9250_ApplyRotation( const Eigen::Matrix3f &c );
uint8 MPU9250_accel_range( void );
uint8 MPU9250_gyro_range( void );
uint8 MPU9250_srd( void );
float MPU9250_accel_x_mps2( void );
float MPU9250_accel_y_mps2( void );
float MPU9250_accel_z_mps2( void );
float MPU9250_gyro_x_radps( void );
float MPU9250_gyro_y_radps( void );
float MPU9250_gyro_z_radps( void );
float MPU9250_mag_x_ut( void );
float MPU9250_mag_y_ut( void );
float MPU9250_mag_z_ut( void );
float MPU9250_die_temperature_c( void );
uint8 MPU9250_dlpf( void );
int MPU9250_CheckStationary( void );
uint32 MPU9250_CheckMagnetDirection( void );

uint32 getSensorKey(void);


#endif  // MPU9250_H

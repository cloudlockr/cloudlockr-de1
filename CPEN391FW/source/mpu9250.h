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

/*------------------- Constants Define ---------------------------------------*/ 

/* Registers */
#define PWR_MGMNT_1_        0x6B
#define H_RESET_            0x80
#define CLKSEL_PLL_         0x01
#define WHOAMI_             0x75
#define ACCEL_CONFIG_       0x1C
#define GYRO_CONFIG_        0x1B
#define ACCEL_CONFIG2_      0x1D
#define CONFIG_             0x1A
#define SMPLRT_DIV_         0x19
#define INT_PIN_CFG_        0x37
#define INT_ENABLE_         0x38
#define INT_DISABLE_        0x00
#define INT_PULSE_50US_     0x00
#define INT_RAW_RDY_EN_     0x01
#define INT_STATUS_         0x3A
#define RAW_DATA_RDY_INT_   0x01
#define USER_CTRL_          0x6A
#define I2C_MST_EN_         0x20
#define I2C_MST_CLK_        0x0D
#define I2C_MST_CTRL_       0x24
#define I2C_SLV0_ADDR_      0x25
#define I2C_SLV0_REG_       0x26
#define I2C_SLV0_CTRL_      0x27
#define I2C_SLV0_DO_        0x63
#define I2C_READ_FLAG_      0x80
#define I2C_SLV0_EN_        0x80
#define EXT_SENS_DATA_00_   0x49

/* AK8963 registers */
#define AK8963_I2C_ADDR_  	0x0C
#define AK8963_HXL_ 		0x03
#define AK8963_CNTL1_ 		0x0A
#define AK8963_PWR_DOWN_  	0x00
#define AK8963_CNT_MEAS1_  	0x12
#define AK8963_CNT_MEAS2_ 	0x16
#define AK8963_FUSE_ROM_ 	0x0F
#define AK8963_CNTL2_ 		0x0B
#define AK8963_RESET_ 		0x01
#define AK8963_ASA_ 		0x10
#define AK8963_WHOAMI_ 		0x00

#define WHOAMI_MPU9250_     0x71
#define WHOAMI_MPU9255_     0x73
#define WHOAMI_AK8963_      0x48


// DlpfBandwidth
#define DLPF_BANDWIDTH_184HZ    0x01
#define DLPF_BANDWIDTH_92HZ     0x02
#define DLPF_BANDWIDTH_41HZ     0x03
#define DLPF_BANDWIDTH_20HZ     0x04
#define DLPF_BANDWIDTH_10HZ     0x05
#define DLPF_BANDWIDTH_5HZ      0x06

// AccelRange
#define ACCEL_RANGE_2G          0x00
#define ACCEL_RANGE_4G          0x08
#define ACCEL_RANGE_8G          0x10
#define ACCEL_RANGE_16G         0x18

// GyroRange
#define GYRO_RANGE_250DPS       0x00
#define GYRO_RANGE_500DPS       0x08
#define GYRO_RANGE_1000DPS      0x10
#define GYRO_RANGE_2000DPS      0x18


/*------------------- Type Define --------------------------------------------*/

/*------------------- Global Data --------------------------------------------*/

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


#endif  // MPU9250_H

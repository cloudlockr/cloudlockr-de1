/*
 * MPU9250.c
 */

/*
 * Description
 * This module is an adoption of MPU9250 sensor device driver Bolder Flight project.
 *
 * Note:
 * SPI0 frequency Will stick to 1 MHz. Runtime frequency variants are not supported.
 *
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "TypeDef.h"
#include "HPS.h"

#include "altera_avalon_spi.h"

// where are these below???
#include "mpu9250.h"


/*------------------- Constants Define ---------------------------------------*/

// Base address of SPI0 registers
#define SPI0_BASE       0xFF202060

#define SLAVE_CHICP_MPU9250     0

// SPI action
#define SPI_READ        0x80;

#define I2C_CLOCK       400000
#define SPI_READ        0x80

/*------------------- Type Define --------------------------------------------*/

/*------------------- Local Data ---------------------------------------------*/

//static uint32 spi_clock_;
static float die_temperature_c_;

/* Configuration */
static uint8 accel_range_;
static uint8 gyro_range_;
static uint8 dlpf_bandwidth_;
static uint8 srd_;

/* Data */
static float accel_scale_, gyro_scale_, mag_scale_[3];
static float temp_scale_ = 333.87f;

static int16 accel_counts[3];
static int16 gyro_counts[3];
static int16 temp_counts;
static int16 mag_counts[3];


/*------------------- Local Function Prototype -------------------------------*/

static bool Mpu9250_WriteRegister(uint8 reg, uint8 data);
static bool Mpu9250_ReadRegisters(uint8 reg, uint8 count, uint8 *data);
static bool Mpu9250_WriteAk8963Register(uint8 reg, uint8 data);
static bool Mpu9250_ReadAk8963Registers(uint8 reg, uint8 count, uint8 *data);


/*------------------- Global Function ----------------------------------------*/

bool MPU9250_Begin( void )
{
    /* Select clock source to gyro */
    if ( !Mpu9250_WriteRegister(PWR_MGMNT_1_, CLKSEL_PLL_)) {
        return false;
    }
    /* Enable I2C master mode */
    if ( !Mpu9250_WriteRegister(USER_CTRL_, I2C_MST_EN_)) {
        return false;
    }
    /* Set the I2C bus speed to 400 kHz */
    if ( !Mpu9250_WriteRegister(I2C_MST_CTRL_, I2C_MST_CLK_)) {
        return false;
    }
    /* Set AK8963 to power down */
    Mpu9250_WriteAk8963Register(AK8963_CNTL1_, AK8963_PWR_DOWN_);
    
    /* Reset the MPU9250 */
    Mpu9250_WriteRegister(PWR_MGMNT_1_, H_RESET_);
  
    /* Wait for MPU-9250 to come back up */
    HPS_msDelay(1);
  
    /* Reset the AK8963 */
    Mpu9250_WriteAk8963Register(AK8963_CNTL2_, AK8963_RESET_);
    /* Select clock source to gyro */
    if ( !Mpu9250_WriteRegister(PWR_MGMNT_1_, CLKSEL_PLL_)) {
        return false;
    }
    /* Check the WHO AM I byte */
    uint8 who_am_i;
    if ( !Mpu9250_ReadRegisters(WHOAMI_, sizeof(who_am_i), &who_am_i)) {
        return false;
    }
    if ( (who_am_i != WHOAMI_MPU9250_) && (who_am_i != WHOAMI_MPU9255_)) {
        return false;
    }
    /* Enable I2C master mode */
    if ( !Mpu9250_WriteRegister(USER_CTRL_, I2C_MST_EN_)) {
        return false;
    }
    /* Set the I2C bus speed to 400 kHz */
    if (!Mpu9250_WriteRegister(I2C_MST_CTRL_, I2C_MST_CLK_)) {
        return false;
    }
    /* Check the AK8963 WHOAMI */
    if (!Mpu9250_ReadAk8963Registers(AK8963_WHOAMI_, sizeof(who_am_i), &who_am_i)) {
        return false;
    }
    if (who_am_i != WHOAMI_AK8963_) {
        return false;
    }
    /* Get the magnetometer calibration */
    /* Set AK8963 to power down */
    if ( !Mpu9250_WriteAk8963Register(AK8963_CNTL1_, AK8963_PWR_DOWN_)) {
        return false;
    }
    HPS_msDelay(100);  // long wait between AK8963 mode changes
  
    /* Set AK8963 to FUSE ROM access */
    if ( !Mpu9250_WriteAk8963Register(AK8963_CNTL1_, AK8963_FUSE_ROM_)) {
        return false;
    }
    HPS_msDelay(100);  // long wait between AK8963 mode changes
  
    /* Read the AK8963 ASA registers and compute magnetometer scale factors */
    uint8 asa_buff[3];
  
    if ( !Mpu9250_ReadAk8963Registers(AK8963_ASA_, sizeof(asa_buff), asa_buff)) {
        return false;
    }

    /* Set AK8963 to power down */
    if ( !Mpu9250_WriteAk8963Register(AK8963_CNTL1_, AK8963_PWR_DOWN_)) {
        return false;
    }
    /* Set AK8963 to 16 bit resolution, 100 Hz update rate */
    if ( !Mpu9250_WriteAk8963Register(AK8963_CNTL1_, AK8963_CNT_MEAS2_)) {
        return false;
    }
    HPS_msDelay(100);  // long wait between AK8963 mode changes
    /* Select clock source to gyro */
    if ( !Mpu9250_WriteRegister(PWR_MGMNT_1_, CLKSEL_PLL_)) {
        return false;
    }
    /* Instruct the MPU9250 to get 7 bytes from the AK8963 at the sample rate */
    uint8 mag_data[7];
    if ( !Mpu9250_ReadAk8963Registers(AK8963_HXL_, sizeof(mag_data), mag_data)) {
        return false;
    }
    /* Set the accel range to 16G by default */
    if ( !MPU9250_ConfigAccelRange(ACCEL_RANGE_16G)) {
        return false;
    }
    /* Set the gyro range to 2000DPS by default*/
    if ( !MPU9250_ConfigGyroRange(GYRO_RANGE_2000DPS)) {
        return false;
    }
    /* Set the Digital Low Pass Filter (DLPF) to 20HZ by default */
    if ( !MPU9250_ConfigDlpf(DLPF_BANDWIDTH_20HZ)) {
        return false;
    }
    /* Set the SRD to 0 by default */
    if ( !MPU9250_ConfigSrd(0))
    {
        return false;
    }
    
    return true;
}

bool MPU9250_EnableDrdyInt( void )
{
  if ( !Mpu9250_WriteRegister(INT_PIN_CFG_, INT_PULSE_50US_)) {
    return false;
  }
  if ( !Mpu9250_WriteRegister(INT_ENABLE_, INT_RAW_RDY_EN_)) {
    return false;
  }
  return true;
}


bool MPU9250_DisableDrdyInt( void )
{
  if ( !Mpu9250_WriteRegister(INT_ENABLE_, INT_DISABLE_)) {
    return false;
  }
  return true;
}


bool MPU9250_ConfigAccelRange( uint8 range )
{
  uint8 requested_range;
  float requested_scale;
  
  /* Check input is valid and set requested range and scale */
  switch (range)
  {
    case ACCEL_RANGE_2G: {
      requested_range = range;
      requested_scale = 2.0f / 32767.5f;
      break;
    }
    case ACCEL_RANGE_4G: {
      requested_range = range;
      requested_scale = 4.0f / 32767.5f;
      break;
    }
    case ACCEL_RANGE_8G: {
      requested_range = range;
      requested_scale = 8.0f / 32767.5f;
      break;
    }
    case ACCEL_RANGE_16G: {
      requested_range = range;
      requested_scale = 16.0f / 32767.5f;
      break;
    }
    default: {
      return false;
    }
  }
  /* Try setting the requested range */
  if ( !Mpu9250_WriteRegister( ACCEL_CONFIG_, requested_range ))
  {
    return false;
  }
  /* Update stored range and scale */
  accel_range_ = requested_range;
  accel_scale_ = requested_scale;
  return true;
}


bool MPU9250_ConfigGyroRange( uint8  range)
{
  uint8 requested_range;
  float requested_scale;
  
  /* Check input is valid and set requested range and scale */
  switch (range)
  {
    case GYRO_RANGE_250DPS: {
      requested_range = range;
      requested_scale = 250.0f / 32767.5f;
      break;
    }
    case GYRO_RANGE_500DPS: {
      requested_range = range;
      requested_scale = 500.0f / 32767.5f;
      break;
    }
    case GYRO_RANGE_1000DPS: {
      requested_range = range;
      requested_scale = 1000.0f / 32767.5f;
      break;
    }
    case GYRO_RANGE_2000DPS: {
      requested_range = range;
      requested_scale = 2000.0f / 32767.5f;
      break;
    }
    default: {
      return false;
    }
  }
  /* Try setting the requested range */
  if ( !Mpu9250_WriteRegister(GYRO_CONFIG_, requested_range) )
  {
    return false;
  }
  /* Update stored range and scale */
  gyro_range_ = requested_range;
  gyro_scale_ = requested_scale;
  return true;
}


bool MPU9250_ConfigSrd(const uint8 srd)
{
    /* Changing the SRD to allow us to set the magnetometer successfully */
    if ( !Mpu9250_WriteRegister(SMPLRT_DIV_, 19) )
    {
        return false;
    }
    
    /* Set the magnetometer sample rate */
    if (srd > 9)
    {
        /* Set AK8963 to power down */
        Mpu9250_WriteAk8963Register(AK8963_CNTL1_, AK8963_PWR_DOWN_);
    
        HPS_usDelay(100);  // long wait between AK8963 mode changes
    
        /* Set AK8963 to 16 bit resolution, 8 Hz update rate */
        if ( !Mpu9250_WriteAk8963Register(AK8963_CNTL1_, AK8963_CNT_MEAS1_) )
        {
            return false;
        }
    
        HPS_usDelay(100);  // long wait between AK8963 mode changes
    
        /* Instruct the MPU9250 to get 7 bytes from the AK8963 at the sample rate */
        uint8 mag_data[7];
        if ( !Mpu9250_ReadAk8963Registers(AK8963_HXL_, sizeof(mag_data), mag_data))
        {
            return false;
        }
    } 
    else
    {
        /* Set AK8963 to power down */
        Mpu9250_WriteAk8963Register(AK8963_CNTL1_, AK8963_PWR_DOWN_);
        
        HPS_msDelay(100);  // long wait between AK8963 mode changes
        
        /* Set AK8963 to 16 bit resolution, 100 Hz update rate */
        if ( !Mpu9250_WriteAk8963Register(AK8963_CNTL1_, AK8963_CNT_MEAS2_) ) 
        {
            return false;
        }
        
        HPS_msDelay(100);  // long wait between AK8963 mode changes
        
        /* Instruct the MPU9250 to get 7 bytes from the AK8963 at the sample rate */
        uint8 mag_data[7];
        if ( !Mpu9250_ReadAk8963Registers(AK8963_HXL_, sizeof(mag_data), mag_data) )
        {
            return false;
        }
    }
    /* Set the IMU sample rate */
    if ( !Mpu9250_WriteRegister(SMPLRT_DIV_, srd) )
    {
        return false;
    }
    srd_ = srd;
    
    return true;
}

bool MPU9250_ConfigDlpf(const uint8 dlpf)
{
    uint8 requested_dlpf;
  
    /* Check input is valid and set requested dlpf */
    switch (dlpf)
    {
        case DLPF_BANDWIDTH_184HZ: {
            requested_dlpf = dlpf;
            break;
        }
        case DLPF_BANDWIDTH_92HZ: {
            requested_dlpf = dlpf;
            break;
        }
        case DLPF_BANDWIDTH_41HZ: {
            requested_dlpf = dlpf;
            break;
        }
        case DLPF_BANDWIDTH_20HZ: {
            requested_dlpf = dlpf;
            break;
        }
        case DLPF_BANDWIDTH_10HZ: {
            requested_dlpf = dlpf;
            break;
        }
        case DLPF_BANDWIDTH_5HZ: {
            requested_dlpf = dlpf;
            break;
        }
        default: {
            return false;
        }
    }
  
    /* Try setting the dlpf */
    if ( !Mpu9250_WriteRegister(ACCEL_CONFIG2_, requested_dlpf) )
    {
        return false;
    }
  
    if ( !Mpu9250_WriteRegister(CONFIG_, requested_dlpf) )
    {
        return false;
    }
  
    /* Update stored dlpf */
    dlpf_bandwidth_ = requested_dlpf;
  
    return true;
}

void MPU9250_DrdyCallback( uint8 int_pin, void (*function)() )
{
}

bool MPU9250_Read( void )
{
    /* Read the data registers */
    uint8 data_buff[22];
  
    if ( !Mpu9250_ReadRegisters(INT_STATUS_, sizeof(data_buff), data_buff) )
    {
        return false;
    }
    
    /* Check if data is ready */
    bool data_ready = (data_buff[0] & RAW_DATA_RDY_INT_);
    if (!data_ready) {
        return false;
    }
    
    /* Unpack the buffer */

    accel_counts[0] = (int16)( (data_buff[1])  << 8 | data_buff[2]  );
    accel_counts[1] = (int16)( (data_buff[3])  << 8 | data_buff[4]  );
    accel_counts[2] = (int16)( (data_buff[5])  << 8 | data_buff[6]  );
    temp_counts     = (int16)( (data_buff[7])  << 8 | data_buff[8]  );
    gyro_counts[0]  = (int16)( (data_buff[9])  << 8 | data_buff[10] );
    gyro_counts[1]  = (int16)( (data_buff[11]) << 8 | data_buff[12] );
    gyro_counts[2]  = (int16)( (data_buff[13]) << 8 | data_buff[14] );
    mag_counts[0]   = (int16)( (data_buff[16]) << 8 | data_buff[15] );
    mag_counts[1]   = (int16)( (data_buff[18]) << 8 | data_buff[17] );
    mag_counts[2]   = (int16)( (data_buff[20]) << 8 | data_buff[19] );

    return true;
}

int MPU9250_CheckStationary( void )
{
    if ( abs((int) gyro_counts[0]) <= 200 && abs((int) gyro_counts[1]) <= 100 && abs((int) gyro_counts[2]) <= 200)
    {
    	// printf("RFS is stationary. ");
    	// printf( "%d %d %d \n", (int) gyro_counts[0], (int) gyro_counts[1], (int) gyro_counts[2] );
    	return 1;

    }
    else
    {
    	// printf("RFS is not stationary!!! 'Tis moving!!! ");
    	// printf( "%d %d %d \n", (int) gyro_counts[0], (int) gyro_counts[1], (int) gyro_counts[2] );
    	return 0;
    }

    /*
    for ( i = 0; i < 3; i++ )
    {
        // printf( "gyro_counts[%d] = %d\n", i, (int) gyro_counts[i] );
        printf( "%d ", (int) gyro_counts[i] );
    }

    printf("\n");
	*/

    /*
    for ( i = 0; i < 3; i++ )
    {
        printf( "mag_counts[%d] = %d\n", i, (int) mag_counts[i] );
    }
     */

}


uint32 MPU9250_CheckMagnetDirection( void )
{
	printf("Raw magnetometer values: %d %d %d =====> ", (int) mag_counts[0], (int) mag_counts[1], (int) mag_counts[2] );

	// North 1: abs(10), abs(80), >=-100 and <= -80

	// int result[3] = {((int) mag_counts[0] / 100) * 100, ((int) mag_counts[1] / 100) * 100, ((int) mag_counts[2] / 100) * 100};
	// printf("%d %d %d \n", result[0], result[1], result[2]);

    uint32 key;
    uint8 byte[4];
    
    int isCorrect = 1;

    if (MPU9250_CheckStationary() == 1)
    {
    	byte[0] = 10;
    }
    else
    {
    	byte[0] = 50;
    	// isCorrect = 0;
    }

    if ( abs((int) mag_counts[0]) <= 40)
	{
    	byte[1] = 10;
	}
	else
	{
		byte[1] = 51;
		isCorrect = 0;
	}

	if (abs((int) mag_counts[1]) <= 90)
	{
		byte[2] = 10;
	}
	else
	{
		byte[2] = 52;
		isCorrect = 0;
	}

	if ( (int) mag_counts[2] <= 50 && (int) mag_counts[2] >= -150)
	{
		byte[3] = 10;
	}
	else
	{
		byte[3] = 53;
		isCorrect = 0;
	}

    key = (byte[3] << 24) | ( byte[2] << 16 ) | ( byte[1] << 16 ) | ( byte[0] << 16 );
    
    if (isCorrect == 1)
    {
    	printf("Correct!, ");
    }
    else
    {
    	printf("Incorrect... ");
    }

    printf("getSensorKey(): %d\n ", key);

	return key;
}

uint32 getSensorKey(void)
{
	// read sensor values
	MPU9250_Read();

	return MPU9250_CheckMagnetDirection();
}

//void  MPU9250_ApplyRotation(const Eigen::Matrix3f &c) {rotation_ = c;}
uint8 MPU9250_accel_range( void )   { return accel_range_; }
uint8 MPU9250_gyro_range( void )    { return gyro_range_;  }
uint8 MPU9250_dlpf( void )          { return dlpf_bandwidth_;}
uint8 MPU9250_srd( void )           { return srd_;}

float MPU9250_die_temperature_c( void )    { return die_temperature_c_; }


/*------------------- Local Function -----------------------------------------*/

static bool Mpu9250_WriteAk8963Register( uint8 reg, uint8 data )
{
    uint8 ret_val;
  
    if ( !Mpu9250_WriteRegister(I2C_SLV0_ADDR_, AK8963_I2C_ADDR_) )
    {
        return false;
    }
    if ( !Mpu9250_WriteRegister(I2C_SLV0_REG_, reg) )
    {
        return false;
    }
    if ( !Mpu9250_WriteRegister(I2C_SLV0_DO_, data) )
    {
        return false;
    }
    if ( !Mpu9250_WriteRegister(I2C_SLV0_CTRL_, I2C_SLV0_EN_ | sizeof(data)) )
    {
        return false;
    }
    if ( !Mpu9250_ReadAk8963Registers(reg, sizeof(ret_val), &ret_val) )
    {
        return false;
    }
    if (data == ret_val)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static bool Mpu9250_ReadAk8963Registers( uint8 reg, uint8 count, uint8 *data )
{
  if ( !Mpu9250_WriteRegister(I2C_SLV0_ADDR_, AK8963_I2C_ADDR_ | I2C_READ_FLAG_) )
  {
    return false;
  }
  if ( !Mpu9250_WriteRegister(I2C_SLV0_REG_, reg) )
  {
    return false;
  }
  if ( !Mpu9250_WriteRegister(I2C_SLV0_CTRL_, I2C_SLV0_EN_ | count) )
  {
    return false;
  }
  
  HPS_msDelay(1);
  
  return Mpu9250_ReadRegisters( EXT_SENS_DATA_00_, count, data );
}

static bool Mpu9250_WriteRegister( uint8 reg, uint8 data )
{
    uint8 ret_val = 0;
    
    // Note: delegate SPI0 module to handle chip selection using ALT_AVALON_SPI_COMMAND_TOGGLE_SS_N flag.
    //
    alt_avalon_spi_command( SPI0_BASE, SLAVE_CHICP_MPU9250, 1, &reg, 0, null, ALT_AVALON_SPI_COMMAND_MERGE );
    alt_avalon_spi_command( SPI0_BASE, SLAVE_CHICP_MPU9250, 1, &data, 0, null, 0 );
    
    HPS_usDelay(20);
    
    Mpu9250_ReadRegisters( reg, sizeof(ret_val), &ret_val );
    
    // Read back to for comfirmation.
    if (data == ret_val)
    {
        return true;
    }
    else 
    {
        return false;
    }
   
}

static bool Mpu9250_ReadRegisters( uint8 reg, uint8 count, uint8 *data )
{
    uint8 cmd;
    
    cmd = reg | SPI_READ;

    // Note: delegate SPI0 module to handle chip selection using ALT_AVALON_SPI_COMMAND_TOGGLE_SS_N flag.
    alt_avalon_spi_command( SPI0_BASE, SLAVE_CHICP_MPU9250, 1, (uint8*)&cmd, count, data, 0 );
    
    return true;
}

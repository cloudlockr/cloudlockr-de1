/**
 * This module contains project wide constants
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

// Max character length for master password
#define MAX_PASSWORD_LENGTH 32

// Max size for filedata sent from bluetooth
#define MAX_FILEDATA_SIZE 0x0200 // 512 bytes

// Bluetooth constants
#define BUFFER_SIZE 2048	  // 512 bytes of file data + 1536 bytes of extra data allowance
#define TIMEOUT_ITER 10000000 // Large number to represent the max number of iterations with no data arriving (~50 sec)
#define MOCK_BLUETOOTH 0

// Number of times to attempt connecting to router
#define HANDSHAKE 5

// Timing constants
#define TIME_FLAG_1MS 0x0001
#define TIME_FLAG_2MS 0x0002
#define TIME_FLAG_3MS 0x0004
#define TIME_FLAG_4MS 0x0008
#define TIME_FLAG_5MS 0x0010
#define TIME_FLAG_10MS 0x0020
#define TIME_FLAG_20MS 0x0040
#define TIME_FLAG_50MS 0x0080
#define TIME_FLAG_100MS 0x0100
#define TIME_FLAG_200MS 0x0210
#define TIME_FLAG_300MS 0x0410
#define TIME_FLAG_400MS 0x0810
#define TIME_FLAG_500MS 0x1000
#define TIME_FLAG_1SEC 0x2000

// MPU9250 constants
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

#endif /* CONSTANTS_H_ */

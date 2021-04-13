/**
 * This module contains project wide memory mapped addresses
 */

#ifndef SOURCE_MEMADDRESS_H_
#define SOURCE_MEMADDRESS_H_

/*------------------- Constants Define -------------------*/
/* verificationService.c */
#define MASTER_PW_ADDR (volatile unsigned *)0x03000000
#define HEX_ADDR (volatile unsigned *)0x03010000

/* UART.c */
#define Wifi_ReceiverFifo (*(volatile unsigned char *)(0xFF210200))
#define Wifi_TransmitterFifo (*(volatile unsigned char *)(0xFF210200))
#define Wifi_InterruptEnableReg (*(volatile unsigned char *)(0xFF210202))
#define Wifi_InterruptIdentificationReg (*(volatile unsigned char *)(0xFF210204))
#define Wifi_FifoControlReg (*(volatile unsigned char *)(0xFF210204))
#define Wifi_LineControlReg (*(volatile unsigned char *)(0xFF210206))
#define Wifi_ModemControlReg (*(volatile unsigned char *)(0xFF210208))
#define Wifi_LineStatusReg (*(volatile unsigned char *)(0xFF21020A))
#define Wifi_ModemStatusReg (*(volatile unsigned char *)(0xFF21020C))
#define Wifi_ScratchReg (*(volatile unsigned char *)(0xFF21020E))
#define Wifi_DivisorLatchLSB (*(volatile unsigned char *)(0xFF210200))
#define Wifi_DivisorLatchMSB (*(volatile unsigned char *)(0xFF210202))

#define Bluetooth_ReceiverFifo (*(volatile unsigned char *)(0xFF210220))
#define Bluetooth_TransmitterFifo (*(volatile unsigned char *)(0xFF210220))
#define Bluetooth_InterruptEnableReg (*(volatile unsigned char *)(0xFF210222))
#define Bluetooth_InterruptIdentificationReg (*(volatile unsigned char *)(0xFF210224))
#define Bluetooth_FifoControlReg (*(volatile unsigned char *)(0xFF210224))
#define Bluetooth_LineControlReg (*(volatile unsigned char *)(0xFF210226))
#define Bluetooth_ModemControlReg (*(volatile unsigned char *)(0xFF210228))
#define Bluetooth_LineStatusReg (*(volatile unsigned char *)(0xFF21022A))
#define Bluetooth_ModemStatusReg (*(volatile unsigned char *)(0xFF21022C))
#define Bluetooth_ScratchReg (*(volatile unsigned char *)(0xFF21022E))
#define Bluetooth_DivisorLatchLSB (*(volatile unsigned char *)(0xFF210220))
#define Bluetooth_DivisorLatchMSB (*(volatile unsigned char *)(0xFF210222))

#define SWITCHES (volatile unsigned *)(0xFF200000)
#define PUSHBUTTONS (volatile unsigned *)(0xFF200010)

#define LEDS (volatile unsigned *)(0xFF200020)
#define HEX0_1 (volatile unsigned *)(0xFF200030)
#define HEX2_3 (volatile unsigned *)(0xFF200040)
#define HEX4_5 (volatile unsigned *)(0xFF200050)

#define GPIO1_DR (volatile uint32 *)(0xFF709000)
#define GPIO1_DDR (volatile uint32 *)(0xFF709004)

/* aesHwAcc.c */
#define AES_ENCRYPT_ADDR (volatile unsigned *)0xFF203000
#define AES_DECRYPT_ADDR (volatile unsigned *)0xFF204000

/* mpu9250.c */
// Base address of SPI0 registers
#define SPI0_BASE       0xFF202060
#endif /* SOURCE_MEMADDRESS_H_ */

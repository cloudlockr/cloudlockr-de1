/*
 * UART.c
 *
 *  Created on: Mar 12, 2021
 *      Author: Jason Bai
 */
 
/*
 * Description
 * UART module.
 */

#include <stdio.h>
#include <string.h>


#include "TypeDef.h"
#include "memAddress.h";
#include "UART.h"

/*------------------- Constants Define -------------------*/
  
// The "unsigned char *" bit of the declaration ensures a pointer to 
// unsigned byte wide data is created, so that when we use it , 
// unsigned bytes of data will be transferred.

// HINT: 
// To write a byte to the FifoControlReg of the UART, we could write
// something like this: 
// RS232_FifoControlReg = 0x55 ;

// TODO: 
// Use DS5 Software environment to: 
// write the following functions in 'C' to drive the simple RS232 port
// HINT: consult vhdl_16550_uart_2_2.pdf from Canvas for details of 
// programming


/**************************************************************************
** Subroutine to initialise the UART Port by writing some data
** to the internal registers.
** Call this function at the start of the program before you attempt
** to read or write to data via the RS232 port
**
** Refer to UART data sheet for details of registers and programming
***************************************************************************/
void UART_Init( UART_ePORT ePort )
{
    if ( ePort == UART_ePORT_WIFI )
    {
        // set bit 7 of Line Control Register to 1, to gain access to the baud rate registers
        Wifi_LineControlReg = 0x80; // 0x80 = 1000 0000

        // set Divisor latch (LSB and MSB) with correct value for required baud rate
        /*
          The baud rate for the WiFi chip should be 115200.
          The baud rate for the BT chip in AT mode should be 38400. In communications mode it should be 115200.
          It could also be different baud rates like 9600, it's kind of a trial and error process.
        */
        // Baud rate divisor value = (frequency of BR_clk) / (desired baud rate x 16)
        // BR_clk = input clock to UART, 50 MHz?
        Wifi_DivisorLatchLSB = (unsigned char)( (50000000) / ( 115200 * 16) ); 
        Wifi_DivisorLatchMSB = (unsigned char)( ( (50000000) / ( 115200 * 16) ) >> 8 ); 

        // set bit 7 of Line control register back to 0 and
        Wifi_LineControlReg = 0x00; // 0x80 = 1000 0000

        // program other bits in that reg for 8 bit data, 1 stop bit, no parity etc
        Wifi_LineControlReg = 0x03; // 0x03 = 0000 0011

        // Reset the Fifo's in the FiFo Control Reg by setting bits 1 & 2
        Wifi_FifoControlReg = 0x06; // 0x06 = 0000 0110

        // Now Clear all bits in the FiFo control registers
        Wifi_FifoControlReg = 0x00; // 0x00 = 0000 0000
    }
    else if ( ePort == UART_ePORT_BLUETOOTH )
    {
        // set bit 7 of Line Control Register to 1, to gain access to the baud rate registers
        Bluetooth_LineControlReg = 0x80; // 0x80 = 1000 0000

        // set Divisor latch (LSB and MSB) with correct value for required baud rate
        /*
            The baud rate for the WiFi chip should be 115200.
            The baud rate for the BT chip in AT mode should be 38400. In communications mode it should be 115200.
            It could also be different baud rates like 9600, it's kind of a trial and error process.
        */
        // Baud rate divisor value = (frequency of BR_clk) / (desired baud rate x 16)
        // BR_clk = input clock to UART, 50 MHz?
        Bluetooth_DivisorLatchLSB = (unsigned char)( (50000000) / ( 115200 * 16) );
        Bluetooth_DivisorLatchMSB = (unsigned char)( ( (50000000) / ( 115200 * 16) ) >> 8 );

        // set bit 7 of Line control register back to 0 and
        Bluetooth_LineControlReg = 0x00; // 0x80 = 1000 0000

        // program other bits in that reg for 8 bit data, 1 stop bit, no parity etc
        Bluetooth_LineControlReg = 0x03; // 0x03 = 0000 0011

        // Reset the Fifo's in the FiFo Control Reg by setting bits 1 & 2
        Bluetooth_FifoControlReg = 0x06; // 0x06 = 0000 0110

        // Now Clear all bits in the FiFo control registers
        Bluetooth_FifoControlReg = 0x00; // 0x00 = 0000 0000
    }
}

/**************************************************************************
** Send a character to the given UART port.
**
***************************************************************************/
int UART_putchar( UART_ePORT ePort, int c)
{
    if ( ePort == UART_ePORT_WIFI )
    {
        // wait for Transmitter Holding Register bit (5) of line status register to be '1'
        // indicating we can write to the device
        while ( !(Wifi_LineStatusReg & (1 << 5)) )
        {
            // wait
        }
  
        // write character to Transmitter fifo register
        Wifi_TransmitterFifo = c;
 
    }
    else if ( ePort == UART_ePORT_BLUETOOTH )
    {
        // wait for Transmitter Holding Register bit (5) of line status register to be '1'
        // indicating we can write to the device
        while ( !(Bluetooth_LineStatusReg & (1 << 5)) )
        {
              // wait
        }

        // write character to Transmitter fifo register
        Bluetooth_TransmitterFifo = c;
    }
    
    // return the character we printed
    return c;
}

/**************************************************************************
** Get a character from the given UART port.
**
***************************************************************************/
int UART_getchar( UART_ePORT ePort )
{
	char newChar;

    if ( ePort == UART_ePORT_WIFI )
    {
        // wait for Data Ready bit (0) of line status register to be '1'
        while ( !(Wifi_LineStatusReg & (1 << 0)) )
        {
            // wait
        }
        
        // read new character from ReceiverFiFo register
        newChar = Wifi_ReceiverFifo;
    }
    else if ( ePort == UART_ePORT_BLUETOOTH )
    {
        // wait for Data Ready bit (0) of line status register to be '1'
        while ( !(Bluetooth_LineStatusReg & (1 << 0)) )
        {
              // wait
        }
      
        // read new character from ReceiverFiFo register
        newChar = Bluetooth_ReceiverFifo;
    }
    
    // return new character
    return newChar;
}


/**************************************************************************
** The following function polls the UART to determine if any character
** has been received. It doesn't wait for one, or read it, it simply tests
** to see if one is available to read from the FIFO.
**
***************************************************************************/
int UART_TestForReceivedData( UART_ePORT ePort )
{
    if ( ePort == UART_ePORT_WIFI )
    {
        // if Wifi_LineStatusReg bit 0 is set to 1
        // return TRUE, otherwise return FALSE
        return ( Wifi_LineStatusReg & (1 << 0));
    }
    else if ( ePort == UART_ePORT_BLUETOOTH )
    {
        // if Wifi_LineStatusReg bit 0 is set to 1
        // return TRUE, otherwise return FALSE
        return (Bluetooth_LineStatusReg & (1 << 0));
    }
    
    return 0;
}

/**************************************************************************
** Remove/flush the UART receiver buffer by removing any unread characters.
**
***************************************************************************/
void UART_Flush( UART_ePORT ePort )
{
    if ( ePort == UART_ePORT_WIFI )
    {
        // while bit 0 of Line Status Register == '1'
        // read unwanted char out of fifo receiver buffer
  
        while ( (Wifi_LineStatusReg & (1 << 0)) )
        {
            Wifi_ReceiverFifo = 0x00;
        }
    }
    else if ( ePort == UART_ePORT_BLUETOOTH )
    {
        // while bit 0 of Line Status Register == ‘1’
        // read unwanted char out of fifo receiver buffer

        while ( (Bluetooth_LineStatusReg & (1 << 0)) )
        {
              Bluetooth_ReceiverFifo = 0x00;
        }
    }
}

void UART_puts( UART_ePORT ePort, char *buffer )
{   
    char *ptr;
    
    if ( buffer == null )
    {
        return;
    }
    
    ptr = buffer;
    while ( *ptr != null )
    {
        UART_putchar( ePort, (int)(*ptr) );
        ptr++;
        HPS_usleep(1);
    }
}

// mode: way of termination
char* UART_gets( UART_ePORT ePort, char* buffer, int length, int mode )
{
    int count = 0;
    
    // Initialize buffer with NULL.
    memset( buffer, NULL, length );
    
    while ( count < (length-1) )
    {
        if ( UART_TestForReceivedData( ePort ) )
	    {
	        buffer[count] = UART_getchar( ePort );

            if ( mode == 0 )
            {
                if ( count > 0 && buffer[count-1] == '\r' && buffer[count] == '\n' )
                {
                    //printf("[e0]");
                    break;
                }
                
                if ( buffer[count] == 0 )
                {
                    //printf("[e1]");
                    break;
                }
            }
            else
            {
                if ( buffer[count] == '\n' || buffer[count] == '\r' || buffer[count] == 0 )
                {
                    break;
                }
            }

            count++;
	    }
        // this affects comm a lot.
        else
        {
            //break;
        }
    }
    
    if ( count == 0 )
    {
        return NULL;
    }
    else
    {
        return ( buffer );
    }
}









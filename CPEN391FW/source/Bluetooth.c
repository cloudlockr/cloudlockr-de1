/*
 * Bluetooth.c
 *
 *  Created on: Mar 12, 2021
 *      Author: Jason Bai
 */

/*
 * Description
 * This module implement wifi related functionalities.
 */

#include <stdio.h>
#include <string.h>

#include "TypeDef.h"
#include "HPS.h"
#include "UART.h"
#include "Bluetooth.h"


#define BUFFER_SIZE 30     // 128 is enough?
static int  bluetooth_Count = 0;
static char bluetooth_Data[BUFFER_SIZE];

typedef enum
{
    STATE_PARSE_MSG = 0,
    STATE_MSG_VALUE,  // may not be needed
    STATE_JSON_PARSE,
} STATE_T;


void BLUETOOTH_Receive( char ch )
{
    int msgType;
    int success;
    bool bReceivedMsg;

    bReceivedMsg = false;
	if ( bluetooth_Count < BUFFER_SIZE )
   	{
            bluetooth_Data[bluetooth_Count] = ch;
            bluetooth_Count++;
            
            if ( ( bluetooth_Count >= 2 ) && 
                 ( bluetooth_Data[bluetooth_Count-1] == '\n' ) && 
                 ( bluetooth_Data[bluetooth_Count-2] == '\r' ) )
            {
                bReceivedMsg = true;
            }
   	}
    else if ( bluetooth_Count >= BUFFER_SIZE )
    {   
        bReceivedMsg = true;
    }
        
    if ( bReceivedMsg ) // respond only if a whole msg has be received or termination condition is met.
    {
        bluetooth_Data[bluetooth_Count] = 0;
        printf( "msg received:\n" );
        printf( bluetooth_Data );
        printf( "\n" );
        
        printf("bluetooth buffer stopped at %d chars\n", bluetooth_Count);
        bluetooth_Count = 0;
        
	    // do stuff here to get some kind of JSON object that has fields with values

	    // use JSON object to figure out what kind of message,
	    // and whether message is valid/successful, etc
        
        if ( strncmp( "blah", bluetooth_Data, 4 ) == 0 )
        {
            msgType = 1;
            success = 1;
        }
        else
        {
            msgType = 2;
            success = 0;
        }
	    

	    if (msgType == 1)
	    {
	        // call other function(s) to do actions
	        UART_puts( UART_ePORT_BLUETOOTH, "\"status\": 1\n" );
	    }
	    else if (msgType == 2)
	    {
            UART_puts( UART_ePORT_BLUETOOTH, "\"failed\": 0\n" );
	    }
    }

}


void BLUETOOTH_Process( void )
{
    char buffer[120];
    static int count = 0;

    sprintf( buffer, "DE1 msg to frontend, count: %i\n", count );
    count++;
    
    UART_puts( UART_ePORT_BLUETOOTH, buffer );

}


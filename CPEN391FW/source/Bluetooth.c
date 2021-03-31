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


#define BUFFER_SIZE 128     // 128 is enough?

static int  bluetooth_Count = 0;
static char bluetooth_Data[BUFFER_SIZE];

typedef enum
{
    STATE_PARSE_MSG = 0,
    STATE_MSG_VALUE,  // may not be needed
    STATE_JSON_PARSE,
    STATE_PARSE_MSG_1, // this and below may not be needed
    STATE_PARSE_MSG_2,
    STATE_PARSE_MSG_3,
    STATE_PARSE_MSG_4,
    STATE_PARSE_MSG_5,
} STATE_T;


void BLUETOOTH_Receive( char ch )
{
    static STATE_T state = STATE_PARSE_MSG;
    
    if (state == STATE_PARSE_MSG)
    {
	if ( bluetooth_Count < BUFFER_SIZE && ch == '\r' )
    	{
            bluetooth_Data[bluetooth_Count++] = ch;      
   	}
	
        else
        {                     
            bluetooth_Count = 0;
	    state = STATE_JSON_PARSE;
        }   
    }
    
    if (state == STATE_JSON_PARSE)
    {
	// do stuff here to get some kind of JSON object that has fields with values
	    
	// use JSON object to figure out what kind of message,
	// and whether message is valid/successful, etc
	int msgType =  1; // or 2, 3, ... 7
	int success = 1; // or 0
	int localEncryptionComponent = 555;
 	char buffer[BUFFER_SIZE];

	if (msgType == 1)
	{
	    // call other function(s) to do actions
	    sprintf( buffer, "\"status\": %i", success);
	    UART_puts( UART_ePORT_BLUETOOTH, buffer );
	}
	if (msgType == 2)
	{
	    // call other function(s) to do actions
            sprintf( buffer, "\"status\": %i, \"localEncryptionComponent\": %i", success, localEncryptionComponent);
	    UART_puts( UART_ePORT_BLUETOOTH, buffer );
	}
	// ... and so on...
	if (msgType == 7)
	{
	    // similar to above, fill in later
	    // call other function(s) to do actions
	}
	    
	// call other function(s) to do actions
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

/*
static BLUETOOTH_CMD bluetooth_ParseCmd( void )
{
	BLUETOOTH_CMD bluetooth_Cmd;

    // go through bluetooth_Data and parse bluetooth commands.
    // ....

	return bluetooth_Cmd;
}
  */

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

/*
typedef enum
{
    BLUETOOTH_CMD_INIT = 0,
    BLUETOOTH_CMD_QUERY,
    // ...
    
} BLUETOOTH_CMD;
  */
  
// static functions
// static BLUETOOTH_CMD bluetooth_ParseCmd( void );

 /*
void BLUETOOTH_Init( void )
{
   // what to initialize?
	memset( bluetooth_Data, 0, sizeof(bluetooth_Data) );
}
*/

// Called by main()

void BLUETOOTH_Receive( char ch )
{
    if ( bluetooth_Count < BUFFER_SIZE )
    {
        bluetooth_Data[bluetooth_Count] = ch;
        
        if ( bluetooth_Count == BUFFER_SIZE )
        {                     
            bluetooth_Count = 0;
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

/*
static BLUETOOTH_CMD bluetooth_ParseCmd( void )
{
	BLUETOOTH_CMD bluetooth_Cmd;

    // go through bluetooth_Data and parse bluetooth commands.
    // ....

	return bluetooth_Cmd;
}
  */

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

#include <string.h>

#define BUFFER_SIZE 128     // 128 is enough?

static int  bluetooth_Count = 0;
static char bluetooth_Data[BUFFER_SIZE];

typedef enum
{
    BLUETOOTH_CMD_INIT = 0,
    BLUETOOTH_CMD_QUERY,
    // ...
    
} BLUETOOTH_CMD;

// static functions
static void bluetooth_Process( void );
static BLUETOOTH_CMD bluetooth_ParseCmd( void );


void Bluetooth_Init( void )
{
   // what to initialize?
	memset( bluetooth_Data, 0, sizeof(bluetooth_Data) );
}

// Called by main()
void Bluetooth_Receive( char ch )
{
    if ( bluetooth_Count < BUFFER_SIZE )
    {
        bluetooth_Data[bluetooth_Count] = ch;
        
        if ( bluetooth_Count == BUFFER_SIZE )
        {
            bluetooth_Process();
            
            bluetooth_Count = 0;
        }
    }
}

static void bluetooth_Process( void )
{   
    BLUETOOTH_CMD bluetooth_Cmd;
    
    bluetooth_Cmd = bluetooth_ParseCmd();

    if ( bluetooth_Cmd == BLUETOOTH_CMD_INIT )
    {
        // what to do...
    }
    else if ( bluetooth_Cmd == BLUETOOTH_CMD_QUERY )
    {
        // what to do...
    }
}

static BLUETOOTH_CMD bluetooth_ParseCmd( void )
{
	BLUETOOTH_CMD bluetooth_Cmd;

    // go through bluetooth_Data and parse bluetooth commands.
    // ....

	return bluetooth_Cmd;
}


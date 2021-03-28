/*
 * WIFI.c
 *
 *  Created on: Mar 12, 2021
 *      Author: Jason Bai
 */

/*
 * Description
 * Wifi module.
 */

#include <stdio.h>
#include <string.h>

#include "TypeDef.h"
#include "HPS.h"
#include "UART.h"
#include "WIFI.h"


/*------------------- Constants Define -------------------*/
#define BUFFER_SIZE 128     // 128 is enough?


/*------------------- Type Define -------------------*/
typedef enum
{
    WIFI_eMSG_INIT = 0,
    WIFI_eMSG_QUERY,
    // ...
    
} WIFI_eMSG;



/*------------------- Local Data -------------------*/
int  wifi_Count = 0;
char wifi_Data[BUFFER_SIZE];

const char *time_server_domain = "demo.terasic.com";
const char *get_time_request = "GET /time/ HTTP/1.1\r\nHost: demo.terasic.com\r\nUser-Agent: terasic-rfs\r\n\r\n";
#if 0
const char *get_time_request =
        "GET /time/ HTTP/1.1\r\n\
Host: demo.terasic.com\r\n\
User-Agent: terasic-rfs\r\n\
\r\n\
";
#endif



/*------------------- Local Function Prototype -------------------*/
static WIFI_eMSG Wifi_ParseMessage( void );
static char*     get_time(char *str);
static bool      esp8266_send_command(const char *cmd);
static bool      esp8266_send_data( const char *data, int length);
static void esp8266_dump_rx( void );



/*------------------- Local Function -------------------*/

char *get_time(char *str)
{
    bool success = true;
    char cmd_buffer[100];
    char buffer[1000];
    
    
    // format string into buffer
	sprintf(cmd_buffer, "AT+CIPSTART=\"TCP\",\"%s\",80", time_server_domain); // start TCP connection at domain with port 80
	success = esp8266_send_command(cmd_buffer);


    if (success)
    {
        sprintf(cmd_buffer, "AT+CIPSEND=%d", strlen(get_time_request)); // specify length of GET command
        success = esp8266_send_command(cmd_buffer);
    }
    
    //HPS_usleep(10);
    
    printf( "--- Sending http command...\n" );
    
    if (success)
    {
        success = esp8266_send_data( get_time_request, strlen(get_time_request) );
    }

    int length = 0;
    if (success) 
    {
        while (1) 
        {
            if ( UART_gets( UART_ePORT_WIFI, buffer, sizeof(buffer), 1 ) != NULL )
            {
                printf( "get_time() Rx: %s", buffer + length );
            
                if (strstr(buffer, "+IPD") != NULL) 
                {
                    #if 1
                    length = strlen(buffer);
                    while (1)
                    {
                	    if ( UART_gets( UART_ePORT_WIFI, buffer + length, sizeof(buffer) - length, 0) != NULL )
                        {
                            //printf( "wifi Rx: %s\n", buffer + length );
                            
                            //if (strcmp(buffer + length, "\r\n") == 0)
                            //    break;
                                
                            //length += strlen(buffer + length);
                            
                            break;
                        }
                    }
                    #endif
                    //break;
                }
            }
        }
        
        while ( UART_gets( UART_ePORT_WIFI, buffer, 9, 1) != NULL )
        {
            printf("time: %s\n", buffer);
        }
    }

    if (success)
    {
        strcpy(str, buffer);
        return str;
    }
    else
    {
        return NULL;
    }
}


// for sending AT command
bool esp8266_send_command(const char *cmd)
{
    int length = 0;
    char buffer[1000];
    
    // Send command to WIFI UART port.
    sprintf( buffer, "%s\r\n", cmd );
    UART_puts( UART_ePORT_WIFI, buffer );
    
    memset( buffer, 0, 1000 );
    
    HPS_usleep( 3000 );
    
    while (1) {
        if ( UART_gets( UART_ePORT_WIFI, buffer + length, sizeof(buffer) - length, 0 ) != NULL)
        {
            if ( strstr(buffer + length, "OK") != NULL )
            {
                printf( "wifi Rx: %s\n", buffer + length );
                //if (strcmp("AT+CWLAP", cmd) == 0) { printf("%s", buffer); }
                return true;
            } else if (strstr(buffer + length, "ERROR") != NULL) {
                printf( "wifi Rx: %s\n", buffer + length );
                return false;
            } else if (strstr(buffer + length, "FAIL") != NULL) {
                printf( "wifi Rx: %s\n", buffer + length );
                return false;
            } else if (strstr(buffer + length, "CLOSED") != NULL) {
                printf( "wifi Rx: %s\n", buffer + length );
                return false;
            }
            
            length += strlen(buffer + length);
        }
    }
    
    return false;
}

/**************************************************************************
** For sending HTTP command
**
***************************************************************************/
bool esp8266_send_data( const char *data, int length)
{
	char buffer[1000];
    int count = 0;

    // Send command to WIFI UART port.
    sprintf( buffer, "%s\r\n", data );
    UART_puts( UART_ePORT_WIFI, buffer );

    while (1)
    {
        if ( UART_gets( UART_ePORT_WIFI, buffer + count, sizeof(buffer) - count, 0 ) != NULL )
        {
            if (strstr(buffer + count, "SEND OK") != NULL)
            {
                printf("%s", buffer);
                return true;
            } else if (strstr(buffer + count, "SEND FAIL") != NULL) {
                printf("%s", buffer);
                return false;
            } else if (strstr(buffer + count, "CLOSED") != NULL) {
                printf("%s", buffer);
                return false;
            }
            count += strlen(buffer + count);
        }
        
        //HPS_usleep(10);
    }
    
    return false;
}

static void esp8266_dump_rx( void )
{
    char ch;

    while ( 1 )
    {
        if ( UART_TestForReceivedData( UART_ePORT_WIFI ) )
	    {
	        ch = UART_getchar( UART_ePORT_WIFI );
            printf( "dump Rx: %c\n", ch );
	    }
        else
        {
            break;
        }
    }
    
    UART_Flush( UART_ePORT_WIFI );
}




/*------------------- API Function -------------------*/

/**************************************************************************
** Initialize WIFI module.
**
***************************************************************************/
bool WIFI_Init( void )
{
    bool bSuccess = true;
    char ssid[]   = "wifi network name";
    char passwd[] = "wifi network password";
    char cmd[100];
    
    #if 0
    - The code below enables digital IO pin of esp8266 module.
    - Is it necessary? If yes, what is this digital IO signal?
    if (reset) {
        IOWR_ALTERA_AVALON_PIO_DATA(PIO_WIFI_RESET_BASE, 0);
        usleep(50);
        IOWR_ALTERA_AVALON_PIO_DATA(PIO_WIFI_RESET_BASE, 1);
        usleep(3 * 1000 * 1000);
        esp8266_dump_rx();
    }
    #endif
    
    esp8266_dump_rx();
    
    #if 0
    // Set up server mode.
    esp8266_send_command("AT+CWSAP_CUR=\"Terasic_RFS\",\"1234567890\",5,3");
    esp8266_send_command("AT+CWMODE_CUR=2");
    esp8266_send_command("AT+CWLIF");
    #endif
    
    
    #if 1
    esp8266_send_command("AT+CWMODE_CUR=1");
    esp8266_send_command("AT+CWLAPOPT=1,0x2");
    
    printf("Connecting to WiFi AP (SSID: %s)\n", ssid);
    
    sprintf(cmd, "AT+CWJAP_CUR=\"%s\",\"%s\"", ssid, passwd);
    bSuccess = esp8266_send_command(cmd);
    if (bSuccess)
    {
        printf("Connect to WiFi AP successfully\n");
    }
    else
    {
        printf("Connect to WiFi AP failed\n");
    }
    #endif

    return bSuccess;
}

/**************************************************************************
** Take each character received and process it if a command is received.
**
** Note: TBD.
**
***************************************************************************/
void WIFI_Receive( char ch )
{
    if ( wifi_Count < BUFFER_SIZE )
    {
        wifi_Data[wifi_Count] = ch;
        
        if ( wifi_Count == BUFFER_SIZE )
        {
            // WIFI processing...
            
            wifi_Count = 0;
        }
    }
}

/**************************************************************************
** Process WIFI messages.
**
***************************************************************************/
void WIFI_Process( void )
{   
    WIFI_eMSG eMsg;
    
    // Get network time:
    char str[100];
    int time;
    int hour, minute, second;
    
    #if 0
    if ( get_time(str) != NULL )
    {
        printf( "Network time: %s\n", str );
        
        #if 0
        if (sscanf(str, "%d:%d:%d", &hour, &minute, &second) == 3) {
            time = hour * 10000 + minute * 100 + second;
            SEG7_Decimal(time, 0);
        }
        #endif
    }
    #endif
    
    #if 0
    - Comment out for now.
    eMsg = Wifi_ParseMessage();

    if ( eMsg == WIFI_eMSG_INIT )
    {
        // what to do...
    }
    else if ( eMsg == WIFI_eMSG_QUERY )
    {
        // what to do...
    }
    #endif
    
}

/**************************************************************************
** Parse WIFI messages.
**
***************************************************************************/
static WIFI_eMSG Wifi_ParseMessage( void )
{
	WIFI_eMSG eMsg;

    // go through wifi_Data and parse wifi commands.
    // ....

	return eMsg;
}


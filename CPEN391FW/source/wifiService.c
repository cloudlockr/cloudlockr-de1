#include "WIFI.h"
#include "UART.h"
#include "HPS.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

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

static bool esp8266_send_command(const char *cmd)
{
    int length = 0;
    char buffer[1000];

    // Send command to WIFI UART port.
    sprintf( buffer, "%s\r\n", cmd );
    UART_puts( UART_ePORT_WIFI, buffer );

    memset( buffer, 0, 1000 );

//    HPS_usleep( 3000 );

    while (1) {
        if ( UART_gets( UART_ePORT_WIFI, buffer + length, sizeof(buffer) - length, 0 ) != NULL)
        {
//        	printf("%s\n", buffer+length);
            if ( strstr(buffer + length, "OK") != NULL )
            {
//                printf( "wifi Rx: %s\n", buffer + length );
//                if (strcmp("AT+CWLAP", cmd) == 0) { printf("%s", buffer); }
                return true;
            } else if (strstr(buffer + length, "ERROR") != NULL) {
//                printf( "wifi Rx: %s\n", buffer + length );
                return false;
            } else if (strstr(buffer + length, "FAIL") != NULL) {
//                printf( "wifi Rx: %s\n", buffer + length );
                return false;
            } else if (strstr(buffer + length, "CLOSED") != NULL) {
//                printf( "wifi Rx: %s\n", buffer + length );
                return false;
            }

            length += strlen(buffer + length);
        }
    }

    return false;
}

static bool esp8266_send_data( const char *data, int length, char *buffer)
{
    int count = 0;

    // Send command to WIFI UART port.
    sprintf( buffer, "%s\r\n", data );
    UART_puts( UART_ePORT_WIFI, buffer );

//    memset( buffer, 0, 1000 );

    //HPS_usleep( 3000 );

//    while (1)
//    {
//        if ( UART_gets( UART_ePORT_WIFI, buffer + count, 1000 - count, 0 ) != NULL )
//        {
//            if (strstr(buffer + count, "SEND OK") != NULL)
//            {
////                printf("%s", buffer);
//                return true;
//            } else if (strstr(buffer + count, "SEND FAIL") != NULL) {
////                printf("%s", buffer);
//                return false;
//            } else if (strstr(buffer + count, "CLOSED") != NULL) {
////                printf("%s", buffer);
//                return false;
//            }
//            count += strlen(buffer + count);
//        }
//
//        //HPS_usleep(10);
//    }

    return true;
}

char* uploadData(char* email, char* fileId, char* packetNumber, char* totalPackets, char* fileData) {
	return NULL;
}

static bool initiate_tcp(char *domain) {
	bool success;
    char cmd_buffer[100];
	sprintf(cmd_buffer, "AT+CIPSTART=\"TCP\",\"%s\",80,7200", domain); // start TCP connection at domain with port 80
	success = esp8266_send_command(cmd_buffer);
	return success;
}

//char* get_wifi_networks( void ) {
//	bool success = true;
//	char buffer[1000];
//	char response[2000];
//
//    esp8266_send_command("AT+CWMODE_CUR=1");
//    HPS_usleep( 3000000 );
//    esp8266_send_command("AT+CWLAPOPT=1,0x2");
//    HPS_usleep( 3000000 );
////    HPS_usleep(1000000);
//    success = esp8266_send_command("AT+CWLAP");
//	if (success) {
//		// +CWLAP:<ecn>, <ssid>, <rssi>, <mac>, <ch>, <freq offset>, <freq calibration>
//		// <ecn>, <rssi>, <mac>, <ch>, <freq offset>, <freq calibration>: not used
//		// <ssid>: name of WiFi network
//
//		// sprintf(response, "{\"status\":1,\"networks\":\"");
//	}
//	return NULL;
//}

int set_wifi_config(char* networkName, char* networkPassword) {
	char cmd[1000];
    esp8266_send_command("AT+CWMODE=3");
    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", networkName, networkPassword);
	return esp8266_send_command(cmd);
}

char* getFileMetadata(char* fileId) {
    char cmd_buffer[100];
    char request[1000];
    char response[1000];
    char body[1000];
    printf("Begin file metadata calls\n");
    sprintf(request, "GET / HTTP/1.1\r\nHost: cloudlockr.herokuapp.com\r\n\r\n", fileId);
//    sprintf(request, "GET /time/ HTTP/1.1\r\nHost: demo.terasic.com\r\nUser-Agent: terasic-rfs\r\n\r\n");
	if (initiate_tcp("cloudlockr.herokuapp.com")) {
//	if (initiate_tcp("demo.terasic.com")) {
        sprintf(cmd_buffer, "AT+CIPSEND=%d", strlen(request)); // specify length of GET command
//        HPS_usleep(60000000);
		if (esp8266_send_command(cmd_buffer)) {
//			HPS_usleep(60000000);
			if (esp8266_send_data( request, strlen(request), response)) {
				// do something with response
//				HPS_usleep(10000000);
				while (1)
				{
					if ( UART_gets( UART_ePORT_WIFI, response, sizeof(response), 1 ) != NULL )
					{
//						printf("%s", response);
						if (strstr(response, "+IPD") != NULL)
						{
							int length = strlen(response);
//							printf("%s", response + length);
							while (1)
							{
								if ( UART_gets( UART_ePORT_WIFI, response + length, sizeof(response) - length, 2) != NULL )
								{
									break;
								}
							}
							break;
						}
					}
				}
//				printf("%s", response);
				while ( UART_gets( UART_ePORT_WIFI, body, 1000, 3) != NULL )
				{
					printf("%s", body);
				}
//				printf("%s", body);
				return "{\"status\": 1}";
			}
			return "{\"status\": 0}";
		}
		return NULL;
	}
	return NULL;
}

char* getBlob(char* fileId, char* blobNumber) {
    char cmd_buffer[100];
    char request[1000];
    char response[1000];
    sprintf(request, "GET /file/%s/%s HTTP/1.1\r\nHost: cloudlockr.herokuapp.com\r\nUser-Agent: terasic-rfs\r\n\r\n", fileId, blobNumber);
	if (initiate_tcp("https://cloudlockr.herokuapp.com/")) {
        sprintf(cmd_buffer, "AT+CIPSEND=%d", strlen(request)); // specify length of GET command
		if (esp8266_send_command(cmd_buffer)) {
			if (esp8266_send_data( request, strlen(request), response )) {
				// do something with response
				return "{\"status\": 1}";
			}
			return "{\"status\": 0}";
		}
		return NULL;
	}
	return NULL;
}


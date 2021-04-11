#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "WIFI.h"
#include "UART.h"
#include "hpsService.h"
#include "JsonParser.h"

static void esp8266_dump_rx(void)
{
    char ch;

    while (1)
    {
        if (UART_TestForReceivedData(UART_ePORT_WIFI))
        {
            ch = UART_getchar(UART_ePORT_WIFI);
            // printf("dump Rx: %c\n", ch);
        }
        else
        {
            break;
        }
    }

    UART_Flush(UART_ePORT_WIFI);
}

static bool esp8266_send_command(const char *cmd)
{
    int length = 0;
    char buffer[1000];

    // Send command to WIFI UART port.
    sprintf(buffer, "%s\r\n", cmd);
    UART_puts(UART_ePORT_WIFI, buffer);

    memset(buffer, 0, 1000);

    while (1)
    {
        if (UART_gets(UART_ePORT_WIFI, buffer + length, sizeof(buffer) - length, 0) != NULL)
        {
            if (strstr(buffer + length, "OK") != NULL)
            {
                return true;
            }
            else if (strstr(buffer + length, "ERROR") != NULL)
            {
                return false;
            }
            else if (strstr(buffer + length, "FAIL") != NULL)
            {
                return false;
            }
            else if (strstr(buffer + length, "CLOSED") != NULL)
            {
                printf("ESP CLOSED\n");
                esp8266_dump_rx();
                return false;
            }

            length += strlen(buffer + length);
        }
    }

    return false;
}

static bool esp8266_send_data(const char *data, int length, char *buffer)
{
    int count = 0;

    // Send command to WIFI UART port.
    sprintf(buffer, "%s\r\n", data);
    UART_puts(UART_ePORT_WIFI, buffer);

    return true;
}

static bool initiate_tcp(char *domain)
{
    bool success;
    char cmd_buffer[100];
    sprintf(cmd_buffer, "AT+CIPSTART=\"TCP\",\"%s\",80,7200", domain); // start TCP connection at domain with port 80
    success = esp8266_send_command(cmd_buffer);
    return success;
}

static bool close_tcp()
{
    bool success;
    char *cmd_buffer = "AT+CIPCLOSE";
    success = esp8266_send_command(cmd_buffer);
    return success;
}

int uploadData(char *fileId, char *packetNumber, char *fileData)
{
    char cmd_buffer[100];
    char request[1000];
    char response[1000];
    printf("Begin upload data call\n");
    sprintf(request, "POST /file/%s/%s HTTP/1.1\r\nHost: cloudlockr.herokuapp.com\r\n\r\n{\"fileData\":\"%s\"}", fileId, packetNumber, fileData);
    if (initiate_tcp("cloudlockr.herokuapp.com"))
    {
        sprintf(cmd_buffer, "AT+CIPSEND=%d", strlen(request)); // specify length of GET command
        if (esp8266_send_command(cmd_buffer))
        {
            if (esp8266_send_data(request, strlen(request), response))
            {
                while (1)
                {
                    if (UART_gets(UART_ePORT_WIFI, response, sizeof(response), 1) != NULL)
                    {
                        if (strstr(response, "+IPD") != NULL)
                        {
                            int length = strlen(response);
                            while (1)
                            {
                                if (UART_gets(UART_ePORT_WIFI, response + length, sizeof(response) - length, 2) != NULL)
                                {
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
                char *body = strstr(response, "\r\n\r\n");
                printf("%s\n", body);
                // while (UART_gets(UART_ePORT_WIFI, response, 9, 1) != NULL)
                // {
                //     printf("%s", response);
                // }
                close_tcp();
                jsmntok_t *tokens = str_to_json(body);
                char **values = get_json_values(body,tokens,1);
                free(tokens);
                return atoi(values[0]);
            }
            printf("Send data failed\n");
            return -1;
        }
        printf("Send command failed\n");
        return -1;
    }
    printf("Initiate tcp failed\n");
    return -1;
}

//char* get_wifi_networks( void ) {
//	bool success = true;
//	char buffer[1000];
//	char response[2000];
//
//    esp8266_send_command("AT+CWMODE_CUR=1");
//    hps_usleep( 3000000 );
//    esp8266_send_command("AT+CWLAPOPT=1,0x2");
//    hps_usleep( 3000000 );
////    hps_usleep(1000000);
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

int set_wifi_config(char *networkName, char *networkPassword)
{
    char cmd[1000];
    int handshake_count = 5;
    int mode_set, connected;

    for (int i = 0; i < handshake_count; i++)
    {
        mode_set = esp8266_send_command("AT+CWMODE=3");
        if (mode_set)
        {
            break;
        }
    }

    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", networkName, networkPassword);
    for (int i = 0; i < handshake_count && mode_set; i++)
    {
        connected = esp8266_send_command(cmd);
        if (connected)
        {
            break;
        }
    }

    return connected;
}

int getFileMetadata(char *fileId)
{
    char cmd_buffer[100];
    char request[1000];
    char response[1000];
    printf("Begin file metadata call\n");
    sprintf(request, "GET /file/%s HTTP/1.1\r\nHost: cloudlockr.herokuapp.com\r\n\r\n", fileId);
    // sprintf(request, "GET /time/ HTTP/1.1\r\nHost: demo.terasic.com\r\nUser-Agent: terasic-rfs\r\n\r\n");
    if (initiate_tcp("cloudlockr.herokuapp.com"))
    // if (initiate_tcp("demo.terasic.com"))
    {
        sprintf(cmd_buffer, "AT+CIPSEND=%d", strlen(request)); // specify length of GET command
        if (esp8266_send_command(cmd_buffer))
        {
            if (esp8266_send_data(request, strlen(request), response))
            {
                while (1)
                {
                    if (UART_gets(UART_ePORT_WIFI, response, sizeof(response), 1) != NULL)
                    {
                        if (strstr(response, "+IPD") != NULL)
                        {
                            int length = strlen(response);
                            while (1)
                            {
                                if (UART_gets(UART_ePORT_WIFI, response + length, sizeof(response) - length, 2) != NULL)
                                {
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
                char *body = strstr(response, "\r\n\r\n");
                printf("%s\n", body);
                // while (UART_gets(UART_ePORT_WIFI, response, 9, 1) != NULL)
                // {
                //     printf("%s", response);
                // }
                close_tcp();
                jsmntok_t *tokens = str_to_json(body);
                char **values = get_json_values(body,tokens,1);
                free(tokens);
                return atoi(values[0]);
            }
            printf("Send data failed\n");
            return -1;
        }
        printf("Send command failed\n");
        return -1;
    }
    printf("Initiate tcp failed\n");
    return -1;
}

char *getBlob(char *fileId, char *blobNumber)
{
    char cmd_buffer[100];
    char request[1000];
    char response[1000];
    printf("Begin get blob call\n");
    sprintf(request, "GET /file/%s/%s HTTP/1.1\r\nHost: cloudlockr.herokuapp.com\r\n\r\n", fileId, blobNumber);
    if (initiate_tcp("cloudlockr.herokuapp.com"))
    {
        sprintf(cmd_buffer, "AT+CIPSEND=%d", strlen(request)); // specify length of GET command
        if (esp8266_send_command(cmd_buffer))
        {
            if (esp8266_send_data(request, strlen(request), response))
            {
                while (1)
                {
                    if (UART_gets(UART_ePORT_WIFI, response, sizeof(response), 1) != NULL)
                    {
                        if (strstr(response, "+IPD") != NULL)
                        {
                            int length = strlen(response);
                            while (1)
                            {
                                if (UART_gets(UART_ePORT_WIFI, response + length, sizeof(response) - length, 2) != NULL)
                                {
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
                char *body = strstr(response, "\r\n\r\n");
                printf("%s\n", body);
                close_tcp();
                jsmntok_t *tokens = str_to_json(body);
                char **values = get_json_values(body,tokens,1);
                free(tokens);
                return values[0];
            }
            printf("Send data failed\n");
            return NULL;
        }
        printf("Send command failed\n");
        return NULL;
    }
    printf("Initiate tcp failed\n");
    return NULL;
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "TypeDef.h"
#include "constants.h"
#include "wifiService.h"
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

int upload_data(char *file_id, int blob_number, char *file_data)
{
    char cmd_buffer[100];
    char blob_number_buf[30];
    sprintf(blob_number_buf, "%d", blob_number);
    char req_body[2 * MAX_FILEDATA_SIZE + 20];
    char request[2 * MAX_FILEDATA_SIZE + 300];
    char response[1000];
    printf("Begin upload data call\n");
    sprintf(req_body, "{\"fileData\":\"%s\"}", file_data);
    sprintf(request, "POST /file/%s/%s HTTP/1.1\r\nHost: cloudlockr.herokuapp.com\r\nContent-Type: application/json; charset=UTF-8\r\nContent-Length: %i\r\n\r\n%s", file_id, blob_number_buf, strlen(req_body), req_body);
    if (initiate_tcp("cloudlockr.herokuapp.com"))
    {
        sprintf(cmd_buffer, "AT+CIPSEND=%d", strlen(request)); // specify length of POST command
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
                char **values = get_json_values(body, tokens, 1);
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

int set_wifi_config(char *networkName, char *networkPassword)
{
    char cmd[200];
    int mode_set, connected;

    for (int i = 0; i < HANDSHAKE; i++)
    {
        mode_set = esp8266_send_command("AT+CWMODE=3");
        if (mode_set)
        {
            break;
        }
    }

    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", networkName, networkPassword);
    for (int i = 0; i < HANDSHAKE && mode_set; i++)
    {
        connected = esp8266_send_command(cmd);
        if (connected)
        {
            printf("Connected to router\n");
            break;
        }
    }

    return connected;
}

int get_file_metadata(char *file_id)
{
    char cmd_buffer[100];
    char request[150];
    char response[1000];
    printf("Begin file metadata call\n");
    sprintf(request, "GET /file/%s HTTP/1.1\r\nHost: cloudlockr.herokuapp.com\r\n\r\n", file_id);
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
                close_tcp();
                jsmntok_t *tokens = str_to_json(body);
                char **values = get_json_values(body, tokens, 1);
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

char *get_blob(char *file_id, int blob_number)
{
    char cmd_buffer[100];
    char blob_number_buf[30];
    sprintf(blob_number_buf, "%d", blob_number);
    char request[150];
    char response[2 * MAX_FILEDATA_SIZE + 1000];
    printf("Begin get blob call\n");
    sprintf(request, "GET /file/%s/%s HTTP/1.1\r\nHost: cloudlockr.herokuapp.com\r\n\r\n", file_id, blob_number_buf);
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
                close_tcp();
                jsmntok_t *tokens = str_to_json(body);
                char **values = get_json_values(body, tokens, 1);
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

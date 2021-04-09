/**
 * This module contains the main function which serves as the entry point to the CloudLockr firmware application.
 */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Application common headers */
#include "TypeDef.h"
#include "memAddress.h"

/* Application module headers */
#include "UART.h"
#include "WIFI.h"
#include "bluetoothService.h"
#include "JsonParser.h"
#include "hexService.h"
#include "hpsService.h"
#include "processingService.h"
#include "verificationService.h"

/*------------------- Local Function Prototype -------------------*/
static void init(void);
static void controller(void);

/*------------------- FUNCTIONS DELCARATIONS TO BE IMPLEMENTED IN OTHER FILES (TODO: REMOVE ONCE COMPLETED) -------------------*/

// char* upload_data(char* email, char* fileId, char* packetNumber, char* totalPackets, char* fileData);
// void download_data(char* localEncrpytionComponent, char* email, char* fileId);
// char* get_wifi_networks( void );
// int set_wifi_config(char* networkName, char* networkPassword);
// int set_device_password(char* password);

/*-------------------------------------------------------------------------------------------------*/

/**
 * Initialize all firmware modules
 */
static void init(void)
{
    // Initialize UART ports.
    UART_Init(UART_ePORT_WIFI);
    UART_Init(UART_ePORT_BLUETOOTH);

    // Initialize WIFI interface.
    //while ( WIFI_Init() == false)
    //WIFI_Init();
    //{
    // We need a sleep function.
    //    HPS_usleep(3 * 1000 * 1000); // 3 seconds
    //}
}

/**
 * Main controller loop
 */
static void controller(void)
{
    // Set seed as current time, used for random display HEX code
    time_t t;
    srand((unsigned)time(&t));
    int i = 0;
    // Controls whether services can be called, ensures correct user control flow
    int state = 0;

    // Controller main loop
    while (1)
    {
        // Wait for a request message to be received
        // char *json_str = bluetooth_wait_for_data();
        // ====================================================================
        char *json_str;
        if (i == 0)
        {
            json_str = "{\"type\":7,\"password\":\"1234567890abc\"}";
            i++;
        }
        else if (i == 1)
        {
            json_str = "{\"type\":1}";
            i++;
        }
        else if (i == 2)
        {
            json_str = "{\"type\":2,\"password\":\"1234567890abc\",\"hex\":\"ABCDEF\"}";
            i++;
        }
        else
        {
            break;
        }
        jsmntok_t *json_tokens = str_to_json(json_str);

        // Check for JSON parsing errors
        if (json_tokens == NULL)
        {
            // Send error response and abort token processing
            // ====================================================================
            // bluetooth_send_status(0);
            continue;
        }

        // Get the first value to determine message_type
        char **message_type_values = get_json_values(json_str, json_tokens, 1);
        long message_type = strtol(message_type_values[0], NULL, 10);
        free_json_values_array(message_type_values, 1);

        // Direct the request to the appropriate handler function (pure functions that take inputs, not the JSON tokens)
        int status = -1;
        char *response_data = NULL;

        char **all_values;
        int expected_num_values = 0;

        switch (message_type)
        {
        case 1:
        {
            // Request to generate and display HEX code, subsequent requests must include the generated HEX code
            if (state >= 1)
            {
                generate_display_hex_code();
                status = 1;
                state = 2;
            }
            else
            {
                // Master password has not been set, reject request
                status = 0;
            }
            break;
        }
        case 2:
        {
            // Request to verify that the user has included the master password and the generated HEX code
            if (state >= 2)
            {
                expected_num_values = 3;
                all_values = get_json_values(json_str, json_tokens, expected_num_values);

                status = verify(all_values[1], all_values[2]);
                state = 3;
            }
            else
            {
                // No HEX code displayed yet, reject request
                status = 0;
            }
            break;
        }
        case 3:
        {
            // Request to upload new encrypted file data to the server for storage
            if (state >= 3)
            {
                expected_num_values = 5;
                all_values = get_json_values(json_str, json_tokens, expected_num_values);

                // TODO: response_data = upload_data(all_values[1], all_values[2], all_values[3], all_values[4]);
                response_data = "{\"status\":1,\"localEncryptionComponent\":\"test\"}"; // TODO: remove, placeholder until above function is implemented
            }
            else
            {
                // User has not been verified yet
                status = 0;
            }
            break;
        }
        case 4:
        {
            if (state >= 3)
            {
                expected_num_values = 3;
                all_values = get_json_values(json_str, json_tokens, expected_num_values);

                // TODO: download_data(all_values[1], all_values[2], all_values[3]);
            }
            else
            {
                // User has not been verified yet
                status = 0;
            }
            break;
        }
        case 5:
        {
            // TODO: response_data = get_wifi_networks();
            response_data = "{\"status\":1,\"networks\":\"[network1,network2]\"}"; // TODO: remove, placeholder until above function is implemented
            break;
        }
        case 6:
        {
            expected_num_values = 3;
            all_values = get_json_values(json_str, json_tokens, expected_num_values);

            // TODO: status = set_wifi_config(allValues[1], allValues[2]);
            status = 1; // TODO: remove, placeholder until above function is implemented
            break;
        }
        case 7:
        {
            expected_num_values = 2;
            all_values = get_json_values(json_str, json_tokens, expected_num_values);

            set_password(all_values[1]);
            status = 1;
            state = 1;
            break;
        }
        default:
        {
            // Send error status if message_type is invalid
            status = 0;
            state = 0;
        }
        }

        // Free values (if they were collected)
        if (expected_num_values > 0)
        {
            free_json_values_array(all_values, expected_num_values);
        }

        // Pause (to prevent response message from being sent too quickly)
        hps_usleep(1 * 1000 * 1000); // ~0.5 second

        // Send the response message (if applicable)
        if (status != -1)
        {
            // Send basic status response
            printf("Status is: %d\n", status);
            // bluetooth_send_status(status);
        }
        else if (response_data != NULL)
        {
            // Send full char message
            printf("Message is: %s\n", response_data);
            // bluetooth_send_message(response_data);
        }
    }

    reset_hex();
    printf(">>>>>>>>>    CloudLockr Firmware end    <<<<<<<<<\n");
}

/**
 * Main function of the CloudLockr firmware application.
 * 
 * Initialize all firmware modules and enter the infinite controller main loop
 */
int main(void)
{
    printf(">>>>>>>>>    CloudLockr Firmware start    <<<<<<<<<\n");
    // ====================================================================
    // init();
    controller();
    return 0;
}

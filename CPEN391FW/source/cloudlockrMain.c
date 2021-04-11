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
    // while ( WIFI_Init() == false)
    // WIFI_Init();
    // {
    // We need a sleep function.
    //    HPS_usleep(3 * 1000 * 1000); // 3 seconds
    // }
}

/**
 * Main controller loop
 */
static void controller(void)
{
    // Set seed as current time, used for random display HEX code
    time_t t;
    srand((unsigned)time(&t));

    // Controls whether services can be called, ensures correct user control flow
    int state = 0;

    // Controller main loop
    while (1)
    {
        // Wait for a request message to be received
        char *json_str = bluetooth_wait_for_data();
        jsmntok_t *json_tokens = str_to_json(json_str);

        // Check for JSON parsing errors
        if (json_tokens == NULL)
        {
            // Send error response and abort token processing
            bluetooth_send_status(0);
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
				// Request to generate and display HEX code
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

					if (status)
					{
						state = 3;
					}
					else
					{
						state = 2;
					}
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
					expected_num_values = 6;
					all_values = get_json_values(json_str, json_tokens, expected_num_values);
					int packet_number = (int)strtol(all_values[2], NULL, 10);
					int total_packets = (int)strtol(all_values[3], NULL, 10);

					response_data = upload(all_values[1], packet_number, total_packets, all_values[4], all_values[5]);
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
				// Request to download encrypted file data from the server and send to the app
				if (state >= 3)
				{
					expected_num_values = 4;
					all_values = get_json_values(json_str, json_tokens, expected_num_values);

					download(all_values[2], all_values[1], all_values[3]);
				}
				else
				{
					// User has not been verified yet
					status = 0;
				}
				break;
			}
			case 6:
			{
				expected_num_values = 3;
				all_values = get_json_values(json_str, json_tokens, expected_num_values);

            status = set_wifi_config(all_values[1], all_values[2]);
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

        free(json_tokens);

        // Free other JSON token values (if they were collected)
        if (expected_num_values > 0)
        {
            free_json_values_array(all_values, expected_num_values);
        }

        // Pause (to prevent response message from being sent too quickly, a nice subtle bug)
        hps_usleep(1 * 1000 * 1000); // ~0.5 second

        // Send the response message (if applicable)
        if (status != -1)
        {
            // Send basic status response
            bluetooth_send_status(status);
        }
        else if (response_data != NULL)
        {
            // Send full char message
            bluetooth_send_message(response_data);
            free(response_data);
        }
    }

    reset_hex();
}

/**
 * Main function of the CloudLockr firmware application.
 * 
 * Initialize all firmware modules and enter the infinite controller main loop
 */
int main(void)
{
    printf(">>>>>>>>>    CloudLockr Firmware start    <<<<<<<<<\n");
    
	init();
    controller();
	
    // int successful = set_wifi_config("networkName", "networkPassword");
    // if (!successful) {
    // 	printf("Couldn't connect to WiFi");
    // 	return 1;
    // }
    // getFileMetadata("783cf156-aa19-4110-8484-732f1b0a1068");
    // getBlob("783cf156-aa19-4110-8484-732f1b0a1068","0");
    // getBlob("783cf156-aa19-4110-8484-732f1b0a1068","1");
    // getBlob("783cf156-aa19-4110-8484-732f1b0a1068","2");
    // getBlob("783cf156-aa19-4110-8484-732f1b0a1068","3");
    // uploadData("783cf156-aa19-4110-8484-732f1b0a1068", "4", "greatestBlob");
    // getBlob("783cf156-aa19-4110-8484-732f1b0a1068","4");

    printf(">>>>>>>>>    CloudLockr Firmware end    <<<<<<<<<\n");
    return 0;
}

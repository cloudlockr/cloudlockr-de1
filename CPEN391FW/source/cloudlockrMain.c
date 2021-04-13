/**
 * This module contains the main function which serves as the entry point to the CloudLockr firmware application.
 */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <typeDef.h>

/* Application common headers */
#include "memAddress.h"
#include "constants.h"

/* Application module headers */
#include "UART.h"
#include "wifiService.h"
#include "bluetoothService.h"
#include "jsonParser.h"
#include "hexService.h"
#include "hpsService.h"
#include "processingService.h"
#include "verificationService.h"
#include "mpu9250.h"

// Local functions
static void init(void);
static void controller(void);

// Global variables
static uint32_t main_u32TimeFlags = 0;
static uint32_t main_freeCount = 0;

/**
 * Keep track of how much time has elapsed, 
 * for periodically calling functions in the controller
 */
void main_UpdateTimeFlag(void)
{
    static uint32_t Count_ms = 0;

    if (hps_elapsed_us(main_freeCount, 1000))
    {
        main_freeCount = *PtimerCount;
        Count_ms++;
        main_u32TimeFlags |= TIME_FLAG_1MS;

        // Update flags
        if ((Count_ms % 2) == 0)
        {
            main_u32TimeFlags |= TIME_FLAG_2MS;
        }

        if ((Count_ms % 3) == 0)
        {
            main_u32TimeFlags |= TIME_FLAG_3MS;
        }

        if ((Count_ms % 4) == 0)
        {
            main_u32TimeFlags |= TIME_FLAG_4MS;
        }

        if ((Count_ms % 5) == 0)
        {
            main_u32TimeFlags |= TIME_FLAG_5MS;
        }

        if ((Count_ms % 10) == 0)
        {
            main_u32TimeFlags |= TIME_FLAG_10MS;
        }

        if ((Count_ms % 20) == 0)
        {
            main_u32TimeFlags |= TIME_FLAG_20MS;
        }

        if ((Count_ms % 50) == 0)
        {
            main_u32TimeFlags |= TIME_FLAG_50MS;
        }

        if ((Count_ms % 100) == 0)
        {
            main_u32TimeFlags |= TIME_FLAG_100MS;
        }

        if ((Count_ms % 200) == 0)
        {
            main_u32TimeFlags |= TIME_FLAG_200MS;
        }

        if ((Count_ms % 300) == 0)
        {
            main_u32TimeFlags |= TIME_FLAG_300MS;
        }

        if ((Count_ms % 400) == 0)
        {
            main_u32TimeFlags |= TIME_FLAG_400MS;
        }

        if ((Count_ms % 500) == 0)
        {
            main_u32TimeFlags |= TIME_FLAG_500MS;
        }

        if ((Count_ms % 1000) == 0)
        {
            main_u32TimeFlags |= TIME_FLAG_1SEC;
        }
    }
}

/**
 * Initialize all firmware modules
 */
static void init(void)
{
    hps_init();

    // Initialize UART ports.
    UART_Init(UART_ePORT_WIFI);
    UART_Init(UART_ePORT_BLUETOOTH);

    if (MPU9250_Begin())
    {
        printf("MPU9250_Begin: success\n");
    }
    else
    {
        printf("MPU9250_Begin: fail\n");
    }

    if (MPU9250_EnableDrdyInt())
    {
        printf("EnableDrdyInt: success\n");
    }
    else
    {
        printf("EnableDrdyInt: fail\n");
    }

    // Set seed as current time, used for random display HEX code
    time_t t;
    srand((unsigned)time(&t));
    reset_hex();
}

/**
 * Main controller loop
 */
static void controller(void)
{
    // Controls whether services can be called, ensures correct user control flow
    int state = 0, password_set = 0, wifi_set = 0;

    // Controller main loop
    while (1)
    {
        // Wait for a request message to be received
        char *json_str = bluetooth_wait_for_data();
        jsmntok_t *json_tokens = str_to_json(json_str);

        main_UpdateTimeFlag();
        // Blink user green LED at 2Hz.
        if (main_u32TimeFlags & TIME_FLAG_500MS)
        {
            hps_toggle_ledg();
        }

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
                    status = 9;
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

                    // TODO: remove this
                    status = 1;

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
                wifi_set = status;
                state = wifi_set && password_set;
                break;
            }
            case 7:
            {
                expected_num_values = 2;
                all_values = get_json_values(json_str, json_tokens, expected_num_values);

                set_password(all_values[1]);
                status = 1;
                password_set = 1;
                state = wifi_set && password_set;
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
        hps_usleep(1 * 1000 * 2000); // ~1 second

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

        // Clear time flags  at the end of each iteration.
        main_u32TimeFlags = 0;
    }
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

    //int successful = set_wifi_config("", "");
    //if (!successful) {
//    	printf("Couldn't connect to WiFi");
//    	return 1;
//    }
//    get_file_metadata("783cf156-aa19-4110-8484-732f1b0a1068");
//    get_blob("783cf156-aa19-4110-8484-732f1b0a1068",0);
//    get_blob("783cf156-aa19-4110-8484-732f1b0a1068",1);
//    get_blob("783cf156-aa19-4110-8484-732f1b0a1068",2);
//    get_blob("783cf156-aa19-4110-8484-732f1b0a1068",3);
//    upload_data("783cf156-aa19-4110-8484-732f1b0a1068", 4, "greatestBlob");
//    get_blob("783cf156-aa19-4110-8484-732f1b0a1068",4);
//
    printf(">>>>>>>>>    CloudLockr Firmware end    <<<<<<<<<\n");
    return 0;
}

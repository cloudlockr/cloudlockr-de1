/**
 * This module contains functions to generate the encryption key
 * and functions to facilitate the upload/download process of file data to the
 * server and back.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "bluetoothService.h"
#include "JsonParser.h"
#include "processingService.h"
#include "verificationService.h"
#include "aesHwacc.h"

/**
 * Function to generate private key.
 * 32 bits of randomly generated number that should be sent back to frontend application.
 * 32 bits from DE1 master password (must be at least 4 characters long).
 * 64 bits from DE1 sensor inputs (switches, WiFi location, NESW orientation, angular velocity)
 *
 * Params:
 * 	wifi_lat		Latitude of geolocation from google maps API
 * 	wifi_long		Longitude of geolocation from google maps API
 *
 * 	key				unsigned char array of size 16, will be filled with private key after function execution
 *
 * Private key will be returned as a 16 elements unsigned char array
 */
void generate_key(unsigned wifi_lat, unsigned wifi_long, unsigned char key[])
{
    // Randomly generated 32 bits
    key[0] = (unsigned char)rand() % 256;
    key[1] = (unsigned char)rand() % 256;
    key[2] = (unsigned char)rand() % 256;
    key[3] = (unsigned char)rand() % 256;

    // 32 bit DE1 master password
    char master_pw[32];
    get_password(master_pw);
    key[4] = (unsigned char)master_pw[0];
    key[5] = (unsigned char)master_pw[1];
    key[6] = (unsigned char)master_pw[2];
    key[7] = (unsigned char)master_pw[3];

    // WiFi location, latitude is between -90 and +90, normalize to 0 to 180,
    // longitude is between -180 and +180, normalize to 0 to 360
    key[8] = (unsigned char)wifi_lat;
    key[9] = (unsigned char)wifi_long;

    // Don't know how NESW, switches, and angular velocity will be like yet, so leave empty
    key[10] = 0;
    key[11] = 0;
    key[12] = 0;
    key[13] = 0;
    key[14] = 0;
    key[15] = 0;
}

/**
 * Function to regenerate private key given inputs.
 * 32 bits of generated number received from frontend application.
 * 32 bits from DE1 master password.
 * 64 bits from DE1 sensor inputs (switches, WiFi location, NESW orientation, angular velocity)
 *
 * Params:
 * 	gen_num			32 bit integer that the frontend application sends, also the first 32 bits of the private key
 * 	wifi_lat		Latitude of geolocation from google maps API
 * 	wifi_long		Longitude of geolocation from google maps API
 *
 * 	key				unsigned char array of size 16, will be filled with the regenerated private key after function execution
 *
 * Private key will be returned as a 16 elements unsigned char array
 */
void regenerate_key(int gen_num, unsigned wifi_lat, unsigned wifi_long, unsigned char key[])
{
    // First 32 bits frontend application
    key[0] = (unsigned char)(gen_num >> 24);
    key[1] = (unsigned char)(gen_num >> 16);
    key[2] = (unsigned char)(gen_num >> 8);
    key[3] = (unsigned char)gen_num;

    // 32 bit DE1 master password
    char master_pw[32];
    get_password(master_pw);
    key[4] = (unsigned char)master_pw[0];
    key[5] = (unsigned char)master_pw[1];
    key[6] = (unsigned char)master_pw[2];
    key[7] = (unsigned char)master_pw[3];

    // WiFi location, latitude is between -90 and +90, normalize to 0 to 180,
    // longitude is between -180 and +180, normalize to 0 to 360
    key[8] = (unsigned char)wifi_lat;
    key[9] = (unsigned char)wifi_long;

    // Don't know how NESW, switches, and angular velocity will be like yet, so leave empty
    key[10] = 0;
    key[11] = 0;
    key[12] = 0;
    key[13] = 0;
    key[14] = 0;
    key[15] = 0;
}

/**
 * Helper function for upload to encrypt fileData.
 * 
 * Params:
 *  key                 unsigned char array containing the encryption key
 *  file_data           char array containing bytes of file data to encrypt
 *  keyexp              int to specify whether to perform key expansion
 *  entire_ciphertext   dynamically allocated unsigned char array to hold the ciphertext
 */
void encrypt_helper(unsigned char key[], char *file_data, int keyexp, unsigned char *entire_ciphertext)
{
    unsigned char plaintext[16], ciphertext[16];
    int pad = 0, i = 0;

    while (i < MAX_FILEDATA_SIZE)
    {
        // Copy file data to plaintext array and pad it with 0 if necessary
        for (int j = 0; j < 16; j++)
        {
            if (file_data[i + j] == '\0')
            {
                // Once null terminating byte is encountered, remaining plaintext will be padded with 0
                pad = 1;
            }

            if (pad)
            {
                plaintext[j] = 0x0;
            }
            else
            {
                plaintext[j] = file_data[i + j];
            }
        }

        // Encrypt the plaintext
        encrypt(key, plaintext, ciphertext, keyexp);

        for (int j = 0; j < 16; j++)
        {
            entire_ciphertext[i + j] = ciphertext[j];
        }

        i += 16;
    }
}

/**
 * Function to upload file data to the server.
 * Calls other services to generate encryption key and encrypt the file data before calling WiFi service to send to server
 *
 * Params:
 *  file_id         char array containing the file_id which specifies which file on the server to send encrypted blobs to
 *  packet_number   int to specify which packet we are currently on
 *  total_packets   int to specify how many packets of fileData we have to collect
 *  file_data       char array containing bytes of file data to encrypt and send to server. Maximum size is MAX_FILEDATA_SOZE
 *
 * Returns the response data to return to the user. Should contain the status and part of the encryption key
 */
char *upload(char *file_id, int packet_number, int total_packets, char *file_data)
{
    unsigned char key[16];
    unsigned wifi_lat = 30;
    unsigned wifi_long = 90;

    // Allocating memory for encrypted file
    unsigned char *entire_ciphertext = (unsigned char *)malloc(sizeof(char) * MAX_FILEDATA_SIZE);

    // TODO: Call Google geolocation API to acquire wifi_lat and wifi_long

    // Generate encryption key and then encrypt file data
    generate_key(wifi_lat, wifi_long, key);
    // SAVE FIRST 4 BYTES AND SEND BACK TO PHONE FOR STORAGE

    char *json_str;
    jsmntok_t *json_tokens;
    int num_fields = 5;
    char **all_values;

    // Multiple packets of file data to receive
    while (total_packets > packet_number)
    {
        encrypt_helper(key, file_data, packet_number == 1, entire_ciphertext);

        if (packet_number > 1)
        {
            free_json_values_array(all_values, num_fields);
        }

        // Notify user that we are ready to receive another packet of fileData
        // bluetooth_send_status(1);

        // Upload encrypted file data to server
        // uploadData(file_id, entire_ciphertext);

        // Receive the next packet of fileData to encrypt and upload
        // json_str = bluetooth_wait_for_data();

        // placeholder json_str
        if (packet_number == 1)
        {
            json_str = "{\"type\":3,\"fileId\":\"123\",\"packetNumber\":2,\"totalPackets\":3,\"fileData\":\"abcdef123456789001010101\"}";
        }
        else if (packet_number == 2)
        {
            json_str = "{\"type\":3,\"fileId\":\"123\",\"packetNumber\":3,\"totalPackets\":3,\"fileData\":\"1234567890abcdef\"}";
        }

        json_tokens = str_to_json(json_str);
        all_values = get_json_values(json_str, json_tokens, num_fields);
        packet_number = (int)strtol(all_values[2], NULL, 10);
        file_data = all_values[4];
    }

    // Last packet of fileData to receive
    encrypt_helper(key, file_data, packet_number, entire_ciphertext);

    // Upload last packet of encrypted file data to server
    // uploadData(file_id, entire_ciphertext);

    if (packet_number > 1)
    {
        free_json_values_array(all_values, num_fields);
    }

    // Forming the response message which contains the success status and part of the encryption key
    char *response_data = (char *)malloc(sizeof(char) * 52);
    strcpy(response_data, "{\"status\":1,\"localEncryptionComponent\":");

    char encryption_component[12];
    encryption_component[0] = '"';
    for (int i = 0; i < 4; i++)
    {
        // Converting key from ascii to hex and then copying to encryption_component
        sprintf((char *)(encryption_component + (i * 2 + 1)), "%02X", key[i]);
    }

    // Finalizaing the string and concatenating
    encryption_component[9] = '"';
    encryption_component[10] = '}';
    encryption_component[11] = '\0';
    strcat(response_data, encryption_component);

    return response_data;
}

char *download(char *file_id, char *key_portion)
{
    return NULL;
}

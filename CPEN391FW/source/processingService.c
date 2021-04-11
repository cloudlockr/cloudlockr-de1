/**
 * This module contains functions to generate the encryption key
 * and functions to facilitate the upload/download process of file data to the
 * server and back.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "memAddress.h"
#include "JsonParser.h"
#include "bluetoothService.h"
#include "processingService.h"
#include "verificationService.h"
#include "aesHwacc.h"
#include "WIFI.h"

// TODO: remove this as it is only for testing
unsigned char encryption_store[3][32];

/**
 * Hash function to generate unique unsigned characters through some arithmetic
 */
unsigned char hash_four_characters(unsigned char char0, unsigned char char1, unsigned char2, unsigned char3)
{
    int combined0 = (int)char0 * char1;
    int combined1 = (int)char2 * char3;
    int combined2 = (int)char1 * char2;
    int combined3 = combined0 + combined1 - combined2 + (int)char0 - (int)char3;

    unsigned char hashed = (unsigned char)combined3;
    return hashed;
}

/**
 * Hash function to generate a unique unsigned character through some arithmetic
 */
unsigned char hash_location(float first, float second, float third)
{
    int combined = (int)(first * second * third - first * second + third - first);
    unsigned char hashed = (unsigned char)combined;
    return hashed;
}

/**
 * Function to generate private key.
 * 32 bits of randomly generated number that should be sent back to frontend application.
 * 32 bits from DE1 master password (must be at least 4 characters long).
 * 64 bits from DE1 sensor inputs (WiFi location, switches, NESW orientation, angular velocity)
 *
 * Params:
 *  location        char array containing the latitude, longitude, and altitude of user location
 * 	key				unsigned char array of size 16, will be filled with private key after function execution
 */
void generate_key(char *location, unsigned char key[])
{
    // Randomly generated 32 bits
    key[0] = (unsigned char)rand() % 256;
    key[1] = (unsigned char)rand() % 256;
    key[2] = (unsigned char)rand() % 256;
    key[3] = (unsigned char)rand() % 256;
    // REMOVE THIS LATER
    key[0] = 0x01;
    key[1] = 0x02;
    key[2] = 0xab;
    key[3] = 0xcd;

    // 32 bit DE1 master password
    char master_pw[32];
    get_password(master_pw);
    key[4] = hash_four_characters(master_pw[0], master_pw[1], master_pw[2], master_pw[3]);
    key[5] = hash_four_characters(master_pw[1], master_pw[2], master_pw[3], master_pw[0]);
    key[6] = hash_four_characters(master_pw[2], master_pw[3], master_pw[0], master_pw[1]);
    key[7] = hash_four_characters(master_pw[3], master_pw[0], master_pw[1], master_pw[2]);

    // Split location into latitude, longitude, and altitude. Then generate unique characters with these inputs
    char location_buffer[30];
    strcpy(location_buffer, location);

    char *latitude = strtok(location_buffer, "|");
    char *longitude = strtok(NULL, "|");
    char *altitude = strtok(NULL, "|");

    float lat_float, long_float, alt_float;
    lat_float = latitude == NULL ? key[4] : strtof(latitude, NULL);
    long_float = longitude == NULL ? key[5] : strtof(longitude, NULL);
    alt_float = altitude == NULL ? key[6] : strtof(altitude, NULL);

    key[8] = hash_location(lat_float, long_float, alt_float);
    key[9] = hash_location(long_float, alt_float, lat_float);
    key[10] = hash_location(alt_float, lat_float, long_float);

    // Get state of switches
    unsigned char switches = (unsigned char)*SWITCHES;
    *LEDS = switches;
    key[11] = switches;

    // Don't know how NESW and angular velocity will be like yet, so leave empty
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
 * 	encryption_component    char array containint the first 32 bits of the private key
 *  location                char array containing the latitude, longitude, and altitude of user location
 * 	key				        unsigned char array of size 16, will be filled with the regenerated private key after function execution
 */
void regenerate_key(char *encryption_component, char *location, unsigned char key[])
{
    // First 32 bits encryption component from the frontend
    int encryption_int = (int)strtol(encryption_component, NULL, 16);
    key[0] = (unsigned char)(encryption_int >> 24);
    key[1] = (unsigned char)(encryption_int >> 16);
    key[2] = (unsigned char)(encryption_int >> 8);
    key[3] = (unsigned char)encryption_int;

    // 32 bit DE1 master password
    char master_pw[32];
    get_password(master_pw);
    key[4] = hash_four_characters(master_pw[0], master_pw[1], master_pw[2], master_pw[3]);
    key[5] = hash_four_characters(master_pw[1], master_pw[2], master_pw[3], master_pw[0]);
    key[6] = hash_four_characters(master_pw[2], master_pw[3], master_pw[0], master_pw[1]);
    key[7] = hash_four_characters(master_pw[3], master_pw[0], master_pw[1], master_pw[2]);

    // Split location into latitude, longitude, and altitude. Then generate unique characters with these inputs
    char location_buffer[30];
    strcpy(location_buffer, location);

    char *latitude = strtok(location_buffer, "|");
    char *longitude = strtok(NULL, "|");
    char *altitude = strtok(NULL, "|");

    float lat_float, long_float, alt_float;
    lat_float = latitude == NULL ? key[4] : strtof(latitude, NULL);
    long_float = longitude == NULL ? key[5] : strtof(longitude, NULL);
    alt_float = altitude == NULL ? key[6] : strtof(altitude, NULL);

    key[8] = hash_location(lat_float, long_float, alt_float);
    key[9] = hash_location(long_float, alt_float, lat_float);
    key[10] = hash_location(alt_float, lat_float, long_float);

    // Get state of switches
    unsigned char switches = (unsigned char)*SWITCHES;
    *LEDS = switches;
    key[11] = switches;

    // Don't know how NESW and angular velocity will be like yet, so leave empty
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
 *  entire_ciphertext   unsigned char array to hold the ciphertext
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
 * Helper function for download to decrypt fileData.
 * 
 * Params:
 *  key                 unsigned char array containing the encryption key
 *  encrypted_data      unsigned char array containing bytes of file data to decrypt
 *  keyexp              int to specify whether to perform key expansion
 *  entire_plaintext    char array to hold the plaintext
 */
void decrypt_helper(unsigned char key[], unsigned char *encrypted_data, int keyexp, char *entire_plaintext)
{
    unsigned char plaintext[16], ciphertext[16];
    int i = 0;

    while (i < MAX_FILEDATA_SIZE)
    {
        // Copy file data to plaintext array and pad it with 0 if necessary
        for (int j = 0; j < 16; j++)
        {
            ciphertext[j] = encrypted_data[i + j];
        }

        // Decrypt the ciphertext
        decrypt(key, ciphertext, plaintext, keyexp);

        for (int j = 0; j < 16; j++)
        {
            entire_plaintext[i + j] = plaintext[j];
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
 *  location                char array containing the latitude, longitude, and altitude of user location
 *  file_data       char array containing bytes of file data to encrypt and send to server. Maximum size is MAX_FILEDATA_SOZE
 *
 * Returns the response data to return to the user. Should contain the status and part of the encryption key
 */
char *upload(char *file_id, int packet_number, int total_packets, char *location, char *file_data)
{
    unsigned char key[16];
    unsigned char entire_ciphertext[MAX_FILEDATA_SIZE];

    // Generate encryption key and then encrypt file data
    generate_key(location, key);

    char *json_str;
    jsmntok_t *json_tokens;
    int num_fields = 6;
    char **all_values;

    // Multiple packets of file data to receive
    while (total_packets > packet_number)
    {
        encrypt_helper(key, file_data, packet_number == 1, entire_ciphertext);

        for (int j = 0; j < MAX_FILEDATA_SIZE; j++)
        {
            encryption_store[packet_number - 1][j] = entire_ciphertext[j];
        }

        if (packet_number > 1)
        {
            free_json_values_array(all_values, num_fields);
        }

        // Notify user that we are ready to receive another packet of fileData
        bluetooth_send_status(1);

        // Upload encrypted file data to server
        char packet_num[sizeof(int)];
        sprintf(packet_num, "%i", packet_number);
        // upload_data(file_id, packet_num, entire_ciphertext);

        // Receive the next packet of fileData to encrypt and upload
        // json_str = bluetooth_wait_for_data();

        // placeholder json_str
        if (packet_number == 1)
        {
            json_str = "{\"type\":3,\"fileId\":\"123\",\"packetNumber\":2,\"totalPackets\":3,\"location\":\"37.422|-122.084|5.285\",\"fileData\":\"abcdef123456789001010101\"}";
        }
        else if (packet_number == 2)
        {
            json_str = "{\"type\":3,\"fileId\":\"123\",\"packetNumber\":3,\"totalPackets\":3,\"location\":\"37.422|-122.084|5.285\",\"fileData\":\"1234567890abcdef\"}";
        }

        json_tokens = str_to_json(json_str);
        all_values = get_json_values(json_str, json_tokens, num_fields);
        free(json_tokens);
        packet_number = (int)strtol(all_values[2], NULL, 10);
        file_data = all_values[5];
    }

    // Last packet of fileData to receive
    encrypt_helper(key, file_data, packet_number == 1, entire_ciphertext);
    for (int j = 0; j < MAX_FILEDATA_SIZE; j++)
    {
        encryption_store[packet_number - 1][j] = entire_ciphertext[j];
    }

    // Upload last packet of encrypted file data to server
    // upload_data(file_id, entire_ciphertext);

    if (packet_number > 1)
    {
        free_json_values_array(all_values, num_fields);
    }

    // Forming the response message which contains the success status and part of the encryption key
    char *response_data = (char *)malloc(sizeof(char) * 52);
    strcpy(response_data, "{\"status\":1,\"localEncryptionComponent\":\"");

    char encryption_component[9];
    for (int i = 0; i < 4; i++)
    {
        // Converting key from ascii to hex and then copying to encryption_component
        sprintf((char *)(encryption_component + (i * 2)), "%02X", key[i]);
    }
    sprintf(response_data, "{\"status\":1,\"localEncryptionComponent\":\"%s\"}", encryption_component);
    printf("%s\n", response_data);

    return response_data;
}

/**
 * Function to download encrypted file data from server and send it to user.
 * Calls other services to regenerate encryption key and decrypt the file data before sending to user.
 * 
 * Params:
 *  file_id                 char array containing the file_id which specifies which file on the server to download from
 *  encryption_component    char array containing part of the encryption key
 *  location                char array containing the latitude, longitude, and altitude of user location
 */
void download(char *file_id, char *encryption_component, char *location)
{
    unsigned char key[16];

    regenerate_key(encryption_component, location, key);

    unsigned char encrypted_data[MAX_FILEDATA_SIZE];
    char entire_plaintext[MAX_FILEDATA_SIZE + 1];
    char *response_data = (char *)malloc(sizeof(char) * (50 + MAX_FILEDATA_SIZE));

    // Generate encryption key and then encrypt file data
    // int total_packets = get_file_metadata(file_id);
    int total_packets = 3;
    int packet_number = 1;
    int status = 0;

    while (total_packets >= packet_number)
    {
        // Get a file blob/packet and decrypt it
        // get_file_blob(file_id, packet_number, encrypted_data);
        // char packet_num[sizeof(int)];
        // sprintf(packet_num, "%i", packet_number);
        // sprintf(encrypted_data, "%s", get_blob(file_id, packet_num - 1));
        // TODO: remove this, only temporary entire_plaintext
        for (int i = 0; i < MAX_FILEDATA_SIZE; i++)
        {
            encrypted_data[i] = encryption_store[packet_number - 1][i];
        }
        decrypt_helper(key, encrypted_data, packet_number == 1, entire_plaintext);
        entire_plaintext[MAX_FILEDATA_SIZE] = '\0';

        // Form response data and send to user
        sprintf(response_data, "{\"packetNumber\":%c,\"totalPackets\":%c,\"fileData\":\"%s\"}", packet_number + '0', total_packets + '0', entire_plaintext);

        bluetooth_send_message(response_data);
        printf("%s\n", response_data);

        // Wait for user to be ready to receive another response
        while (!status && total_packets > packet_number)
        {
            // char *json_str = bluetooth_wait_for_data();
            // jsmntok_t *json_tokens = str_to_json(json_str);
            // char **all_values = get_json_values(json_str, json_tokens, 1);
            // status = (int)strtol(all_values[0], NULL, 10);
            // free(json_tokens);
            // free_json_values_array(all_values, 1);
            status = 1;
        }

        packet_number++;
    }

    free(response_data);
}

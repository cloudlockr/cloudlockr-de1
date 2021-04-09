#include <stdlib.h>
#include "constants.h"
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
void generateKey(unsigned wifi_lat, unsigned wifi_long, unsigned char key[])
{
    // Randomly generated 32 bits
    key[0] = (unsigned char)rand() % 256;
    key[1] = (unsigned char)rand() % 256;
    key[2] = (unsigned char)rand() % 256;
    key[3] = (unsigned char)rand() % 256;

    // 32 bit DE1 master password
    char master_pw[32];
    getPassword(master_pw);
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
void regenerateKey(int gen_num, unsigned wifi_lat, unsigned wifi_long, unsigned char key[])
{
    // First 32 bits frontend application
    key[0] = (unsigned char)(gen_num >> 24);
    key[1] = (unsigned char)(gen_num >> 16);
    key[2] = (unsigned char)(gen_num >> 8);
    key[3] = (unsigned char)gen_num;

    // 32 bit DE1 master password
    char master_pw[32];
    getPassword(master_pw);
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
 * Function to upload file data to the server.
 * Calls other services to generate encryption key and encrypt the file data before calling WiFi service to send to server
 *
 * Params:
 *  fileId          char array containing the fileId which specifies which file on the server to send encrypted blobs to
 *  packetNumber    int to specify which packet we are currently on
 *  totalPackets    int to specify how many packets of fileData we have to collect
 *  fileData        char array containing bytes of file data to encrypt and send to server. Maximum size 1MB
 *
 * Returns SOMETHING
 */
int upload(char *fileId, int packetNumber, int totalPackets, char *fileData)
{
    unsigned char key[16], plaintext[16], ciphertext[16];
    unsigned wifi_lat = 30;
    unsigned wifi_long = 90;

    // TODO: Call Google geolocation API to acquire wifi_lat and wifi_long

    // Generate encryption key and then encrypt file data
    generateKey(wifi_lat, wifi_long, key);
    // SAVE FIRST 4 BYTES AND SEND BACK TO PHONE FOR STORAGE

    int i = 0;
    while (fileData[i] != '\0' && i < MAX_FILEDATA_SIZE)
    {
        // Copy file data to plaintext array and pad it with 0 if necessary
        for (int j = 0; j < 16; j++)
        {
            if (fileData[i + j] != '\0')
            {
                plaintext[j] = fileData[i + j];
            }
            else
            {
                plaintext[j] = 0x0;
            }
        }

        // Encrypt the plaintext
        encrypt(key, plaintext, ciphertext, 1);

        i += 16;
    }
}

int download()
{
}

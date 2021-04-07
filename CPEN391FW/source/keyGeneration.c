/*
 * keyGeneration.c
 *
 * This module contains the function which implements the private key generation algorithm.
 * Inputs to the algorithm includes DE1 master password, and specific DE1 inputs such as
 * switches ON/OFF sequence, WiFi location positioning, NESW orientation from magnetometer,
 * and angular velocity from gyroscope.
 *
 *  Created on: April 1, 2021
 *      Author: dannsy
 */
#include <stdlib.h>

/**
 * Function to generate private key.
 * 32 bits of randomly generated number that should be sent back to frontend application.
 * 32 bits from DE1 master password (must be at least 4 characters long).
 * 64 bits from DE1 sensor inputs (switches, WiFi location, NESW orientation, angular velocity)
 *
 * Params:
 * 	master_pw		Char array of the master password
 * 	wifi_lat		Latitude of geolocation from google maps API
 * 	wifi_long		Longitude of geolocation from google maps API
 *
 * 	key				unsigned char array of size 16, will be filled with private key after function execution
 *
 * Private key will be returned as a 16 elements unsigned char array
 */
void generateKey(char *master_pw, unsigned wifi_lat, unsigned wifi_long, unsigned char key[])
{
    // Randomly generated 32 bits
    key[0] = (unsigned char)rand() % 256;
    key[1] = (unsigned char)rand() % 256;
    key[2] = (unsigned char)rand() % 256;
    key[3] = (unsigned char)rand() % 256;

    // 32 bit DE1 master password
    key[4] = (unsigned char)master_pw[0];
    key[5] = (unsigned char)master_pw[1];
    key[6] = (unsigned char)master_pw[2];
    key[7] = (unsigned char)master_pw[3];

    // WiFi location, latitude is between -90 and +90, longitude is between -180 and +180
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
 * 	master_pw		Char array of the master password
 * 	wifi_lat		Latitude of geolocation from google maps API
 * 	wifi_long		Longitude of geolocation from google maps API
 *
 * 	key				unsigned char array of size 16, will be filled with the regenerated private key after function execution
 *
 * Private key will be returned as a 16 elements unsigned char array
 */
void regenerateKey(int gen_num, char *master_pw, unsigned wifi_lat, unsigned wifi_long, unsigned char key[])
{
    // First 32 bits frontend application
    key[0] = (unsigned char)(gen_num >> 24);
    key[1] = (unsigned char)(gen_num >> 16);
    key[2] = (unsigned char)(gen_num >> 8);
    key[3] = (unsigned char)gen_num;

    // 32 bit DE1 master password
    key[4] = (unsigned char)master_pw[0];
    key[5] = (unsigned char)master_pw[1];
    key[6] = (unsigned char)master_pw[2];
    key[7] = (unsigned char)master_pw[3];

    // WiFi location, latitude is between -90 and +90, longitude is between -180 and +180
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

/*
 * verificationService.c
 *
 *  Created on: Apr 4, 2021
 *      Author: Andrew C
 */

/*
 * Description
 * Functions for verification.
 */

#include <stdlib.h>
#include "constants.h"
#include "memAddress.h"
#include "verificationService.h"

/**
 * Function to set the master password, takes a user defined password
 * and writes to master password memory address.
 * The user defined password must be between 4 to 32 characters long.
 * 
 * Params:
 *  password		char array containing the password to set
 */
void set_password(char *password)
{
    int i = 0;

    while (password[i] != '\0' && i < MAX_PASSWORD_LENGTH)
    {
        *(MASTER_PW_ADDR + i) = password[i];
        i++;
    }
    // Ensure master password is null terminated
    *(MASTER_PW_ADDR + i) = '\0';
}

/**
 * Function to get the master password.
 * 
 * Params:
 *  password		char array to write the master password to
 */
void get_password(char *password)
{
    int i = 0;
    char master_pw_char;

    do
    {
        master_pw_char = *(MASTER_PW_ADDR + i);
        password[i] = master_pw_char;
        i++;
    } while (master_pw_char != '\0' && i < MAX_PASSWORD_LENGTH);
}

/**
 * Verifies whether the password provided by user matches the master password,
 * and whether the provided hex matches the DE1 HEX display
 * 
 * Params:
 * 	password		char array of the user input password
 *  hex             char array of the user input HEX
 * 
 * If both password and hex are correct, return 1. Otherwise return 0
 */
int verify(char *password, char *hex)
{
    int verified = 1;
    int i = 0;
    char master_pw_char, pw_char;

    // Verify master password
    do
    {
        master_pw_char = *(MASTER_PW_ADDR + i);
        pw_char = password[i];

        if (master_pw_char != pw_char)
        {
            verified = 0;
            break;
        }
        i++;
    } while (master_pw_char != '\0' && pw_char != '\0' && i < MAX_PASSWORD_LENGTH);

    // Verify HEX display only if password matches
    if (verified)
    {
        unsigned input_hex_code = (unsigned)strtol(hex, NULL, 16);
        unsigned correct_hex_code = *HEX_ADDR;
        if (correct_hex_code != input_hex_code)
        {
            verified = 0;
        }
    }

    return verified;
}

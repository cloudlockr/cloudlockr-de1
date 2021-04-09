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
void setPassword(char *password)
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
void getPassword(char *password)
{
    int i = 0;
    char masterPwChar;

    do
    {
        masterPwChar = *(MASTER_PW_ADDR + i);
        password[i] = masterPwChar;
        i++;
    } while (masterPwChar != '\0' && i < MAX_PASSWORD_LENGTH);
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
    char masterPwChar, pwChar;

    // Verify master password
    do
    {
        masterPwChar = *(MASTER_PW_ADDR + i);
        pwChar = password[i];

        if (masterPwChar != pwChar)
        {
            verified = 0;
            break;
        }
        i++;
    } while (masterPwChar != '\0' && pwChar != '\0' && i < MAX_PASSWORD_LENGTH);

    // Verify HEX display only if password matches
    if (verified)
    {
        unsigned inputHexCode = (unsigned)strtol(hex, NULL, 16);
        unsigned correctHexCode = *HEX_ADDR;
        if (correctHexCode != inputHexCode)
        {
            verified = 0;
        }
    }

    return verified;
}

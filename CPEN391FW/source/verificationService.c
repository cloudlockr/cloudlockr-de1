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
#include "memAddress.h"
#include "verificationService.h"

/*------------------- Constants Define -------------------*/

/*------------------- Type Define --------------------------------------------*/
/*------------------- Local Data ---------------------------------------------*/
/*------------------- Local Function Prototype -------------------------------*/
/*------------------- Local Function -----------------------------------------*/
/*------------------- API Function -------------------------------------------*/

/**************************************************************************
** Takes a 32 character password, and copies it to the master password
** address.
**
** Parameters:
**     password      char pointer to the password string
**
***************************************************************************/
void setPassword( char *password )
{
	for ( unsigned char i = 0; i < 32; i++ )
	{
		*(MASTER_PW_ADDR + i) = *(password + i);
	}
}


/**************************************************************************
** Verifies whether the password provided by user matches the master password,
** and whether the provided hex matches the DE1 HEX display
**
** Parameters:
**     password      char pointer to the user input password string
**     hex           char pointer to the user input hex string
**
***************************************************************************/
int verify( char *password, char *hex )
{
	int verified = 1;

	// Verify master password
	for ( unsigned char i = 0; i < 32; i++ )
	{
		if (*(MASTER_PW_ADDR + i) != *(password + i)) {
			verified = 0;
		}
	}

	// Verify HEX display
	unsigned inputHexCode = (unsigned) strtol(hex, NULL, 16);
	unsigned correctHexCode = *HEX_ADDR;
	if (correctHexCode != inputHexCode) {
		verified = 0;
	}

	return verified;
}

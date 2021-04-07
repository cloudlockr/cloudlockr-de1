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

#include "memAddress.h";

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
** Copies the master password to the password string.
**
** Parameters:
**     password      char pointer to the password string
**
***************************************************************************/
void getPassword( char *password )
{
	for ( unsigned char i = 0; i < 32; i++ )
	{
		*(password + i) = *(MASTER_PW_ADDR + i);
	}
}


/**************************************************************************
** thingy
**
** Parameters:
**     password      int *me
**     hex           char *you
**
***************************************************************************/
//bool verify(password, hex)

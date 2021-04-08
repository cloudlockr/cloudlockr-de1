/*
 * verificationService.h
 *
 *  Created on: Apr 4, 2021
 *      Author: Andrew C
 */

#ifndef VERIFICATIONSERVICE_H_
#define VERIFICATIONSERVICE_H_


/*------------------- Type Define -------------------*/
/*------------------- Function Prototype -------------------*/
void setPassword( char *password );
int verify( char *password, char *hex );

#endif /* VERIFICATIONSERVICE_H_ */

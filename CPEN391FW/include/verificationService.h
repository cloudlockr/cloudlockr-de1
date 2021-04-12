/**
 * This module contains function declarations for verificationService.c
 */

#ifndef VERIFICATIONSERVICE_H_
#define VERIFICATIONSERVICE_H_

void set_password(char *password);
void get_password(char *password);
int verify(char *password, char *hex);

#endif /* VERIFICATIONSERVICE_H_ */

/*
 * aesHwacc.h
 *
 * This module contains functions declarations for aesHwacc.c
 *
 *  Created on: April 1, 2021
 *      Author: dannsy
 */

void encrypt(unsigned char key[], unsigned char plaintext[], unsigned char ciphertext[], int keyexp);
void decrypt(unsigned char key[], unsigned char ciphertext[], unsigned char plaintext[], int keyexp);

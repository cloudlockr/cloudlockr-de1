/**
 * This module contains function declarations for aesHwacc.c
 */

#ifndef AESHWACC_H_
#define AESHWACC_H_

void encrypt(unsigned char key[], unsigned char plaintext[], unsigned char ciphertext[], int keyexp);
void decrypt(unsigned char key[], unsigned char ciphertext[], unsigned char plaintext[], int keyexp);

#endif /* AESHWACC_H_ */

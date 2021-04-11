/**
 * This module contains functions for starting and reading the
 * hardware accelerated AES encryption/decryption modules.
 */

#include "memAddress.h"
#include "aesHwacc.h"

/**
  * Function to call Avalon memory mapped custom component AES encryption module.
  *
  * Params:
  * 	key			unsigned char array of 16 elements, each element being 8 bits of the encryption key
  * 	plaintext	unsigned char array of 16 elements, each element being 8 bits of the plaintext
  * 	ciphertext 	unsigned char array of size 16, will be filled with ciphertext after function execution
  * 	keyexp		int/boolean to specify whether to perform key expansion (only need to be true once for each encryption)
  */
void encrypt(unsigned char key[], unsigned char plaintext[], unsigned char ciphertext[], int keyexp)
{
    unsigned word4 = (plaintext[12] << 24) + (plaintext[13] << 16) + (plaintext[14] << 8) + plaintext[15];
    unsigned word5 = (plaintext[8] << 24) + (plaintext[9] << 16) + (plaintext[10] << 8) + plaintext[11];
    unsigned word6 = (plaintext[4] << 24) + (plaintext[5] << 16) + (plaintext[6] << 8) + plaintext[7];
    unsigned word7 = (plaintext[0] << 24) + (plaintext[1] << 16) + (plaintext[2] << 8) + plaintext[3];

    // Write plaintext to AES encryption module
    *(AES_ENCRYPT_ADDR + 4) = word4;
    *(AES_ENCRYPT_ADDR + 5) = word5;
    *(AES_ENCRYPT_ADDR + 6) = word6;
    *(AES_ENCRYPT_ADDR + 7) = word7;

    if (keyexp)
    {
        // Input key and perform key expansion
        unsigned word0 = (key[12] << 24) + (key[13] << 16) + (key[14] << 8) + key[15];
        unsigned word1 = (key[8] << 24) + (key[9] << 16) + (key[10] << 8) + key[11];
        unsigned word2 = (key[4] << 24) + (key[5] << 16) + (key[6] << 8) + key[7];
        unsigned word3 = (key[0] << 24) + (key[1] << 16) + (key[2] << 8) + key[3];

        // Write key to AES encryption module
        *(AES_ENCRYPT_ADDR + 0) = word0;
        *(AES_ENCRYPT_ADDR + 1) = word1;
        *(AES_ENCRYPT_ADDR + 2) = word2;
        *(AES_ENCRYPT_ADDR + 3) = word3;

        *(AES_ENCRYPT_ADDR + 8) = (unsigned)0;
    }
    else
    {
        // Don't perform key expansion
        *(AES_ENCRYPT_ADDR + 9) = (unsigned)0;
    }

    unsigned cipher0 = *(AES_ENCRYPT_ADDR + 0);
    unsigned cipher1 = *(AES_ENCRYPT_ADDR + 1);
    unsigned cipher2 = *(AES_ENCRYPT_ADDR + 2);
    unsigned cipher3 = *(AES_ENCRYPT_ADDR + 3);

    for (int i = 0; i < 4; i++)
    {
        ciphertext[i] = cipher3 >> (8 * (3 - i));
        ciphertext[i + 4] = cipher2 >> (8 * (3 - i));
        ciphertext[i + 8] = cipher1 >> (8 * (3 - i));
        ciphertext[i + 12] = cipher0 >> (8 * (3 - i));
    }
}

/**
 * Function to call Avalon memory mapped custom component AES decryption module.
 *
 * Params:
 * 	key			unsigned char array of 16 elements, each element being 8 bits of the encryption key
 * 	ciphertext	unsigned char array of 16 elements, each element being 8 bits of the ciphertext
 * 	plaintext 	unsigned char array of size 16, will be filled with plaintext after function execution
 * 	keyexp		int/boolean to specify whether to perform key expansion (only need to be true once for each decryption)
 */
void decrypt(unsigned char key[], unsigned char ciphertext[], unsigned char plaintext[], int keyexp)
{
    unsigned word4 = (ciphertext[12] << 24) + (ciphertext[13] << 16) + (ciphertext[14] << 8) + ciphertext[15];
    unsigned word5 = (ciphertext[8] << 24) + (ciphertext[9] << 16) + (ciphertext[10] << 8) + ciphertext[11];
    unsigned word6 = (ciphertext[4] << 24) + (ciphertext[5] << 16) + (ciphertext[6] << 8) + ciphertext[7];
    unsigned word7 = (ciphertext[0] << 24) + (ciphertext[1] << 16) + (ciphertext[2] << 8) + ciphertext[3];

    // Write plaintext to AES decryption module
    *(AES_DECRYPT_ADDR + 4) = word4;
    *(AES_DECRYPT_ADDR + 5) = word5;
    *(AES_DECRYPT_ADDR + 6) = word6;
    *(AES_DECRYPT_ADDR + 7) = word7;

    if (keyexp)
    {
        // Input key and perform key expansion
        unsigned word0 = (key[12] << 24) + (key[13] << 16) + (key[14] << 8) + key[15];
        unsigned word1 = (key[8] << 24) + (key[9] << 16) + (key[10] << 8) + key[11];
        unsigned word2 = (key[4] << 24) + (key[5] << 16) + (key[6] << 8) + key[7];
        unsigned word3 = (key[0] << 24) + (key[1] << 16) + (key[2] << 8) + key[3];

        // Write key to AES decryption module
        *(AES_DECRYPT_ADDR + 0) = word0;
        *(AES_DECRYPT_ADDR + 1) = word1;
        *(AES_DECRYPT_ADDR + 2) = word2;
        *(AES_DECRYPT_ADDR + 3) = word3;

        *(AES_DECRYPT_ADDR + 8) = (unsigned)0;
    }
    else
    {
        // Don't perform key expansion
        *(AES_DECRYPT_ADDR + 9) = (unsigned)0;
    }

    unsigned plain0 = *(AES_DECRYPT_ADDR + 0);
    unsigned plain1 = *(AES_DECRYPT_ADDR + 1);
    unsigned plain2 = *(AES_DECRYPT_ADDR + 2);
    unsigned plain3 = *(AES_DECRYPT_ADDR + 3);

    for (int i = 0; i < 4; i++)
    {
        plaintext[i] = plain3 >> (8 * (3 - i));
        plaintext[i + 4] = plain2 >> (8 * (3 - i));
        plaintext[i + 8] = plain1 >> (8 * (3 - i));
        plaintext[i + 12] = plain0 >> (8 * (3 - i));
    }
}

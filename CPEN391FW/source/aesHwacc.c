/*
 * aes_hwacc.c
 *
 * This module contains functions for starting and reading the
 * hardware accelerated AES encryption/decryption modules.
 * It is meant to be called from the main ARM controller.
 *
 *  Created on: April 1, 2021
 *      Author: dannsy
 */
#include <stdio.h>
#include "memAddress.h"

/**
 * Function to call Avalon memory mapped custom component AES encryption module.
 *
 * Params:
 * 	key			unsigned char array of 16 elements, each element being 8 bits of the encryption key
 * 	plaintext	unsigned char array of 16 elements, each element being 8 bits of the plaintext
 * 	ciphertext 	unsigned char array of size 16, will be filled with ciphertext after function execution
 * 	keyexp		int/boolean to specify whether to perform key expansion (only need to be true once for each encryption)
 */
void aesEncrypt(unsigned char key[], unsigned char plaintext[], unsigned char ciphertext[], int keyexp)
{
    unsigned word4 = (plaintext[12] << 24) + (plaintext[13] << 16) + (plaintext[14] << 8) + plaintext[15];
    unsigned word5 = (plaintext[8] << 24) + (plaintext[9] << 16) + (plaintext[10] << 8) + plaintext[11];
    unsigned word6 = (plaintext[4] << 24) + (plaintext[5] << 16) + (plaintext[6] << 8) + plaintext[7];
    unsigned word7 = (plaintext[0] << 24) + (plaintext[1] << 16) + (plaintext[2] << 8) + plaintext[3];

    // Write plaintext to AES encryption module
    *(aes_encrypt_addr + 4) = word4;
    *(aes_encrypt_addr + 5) = word5;
    *(aes_encrypt_addr + 6) = word6;
    *(aes_encrypt_addr + 7) = word7;

    if (keyexp)
    {
        // Input key and perform key expansion
        unsigned word0 = (key[12] << 24) + (key[13] << 16) + (key[14] << 8) + key[15];
        unsigned word1 = (key[8] << 24) + (key[9] << 16) + (key[10] << 8) + key[11];
        unsigned word2 = (key[4] << 24) + (key[5] << 16) + (key[6] << 8) + key[7];
        unsigned word3 = (key[0] << 24) + (key[1] << 16) + (key[2] << 8) + key[3];

        // Write key to AES encryption module
        *(aes_encrypt_addr + 0) = word0;
        *(aes_encrypt_addr + 1) = word1;
        *(aes_encrypt_addr + 2) = word2;
        *(aes_encrypt_addr + 3) = word3;

        *(aes_encrypt_addr + 8) = (unsigned)0;
    }
    else
    {
        // Don't perform key expansion
        *(aes_encrypt_addr + 9) = (unsigned)0;
    }

    unsigned cipher0 = *(aes_encrypt_addr + 0);
    unsigned cipher1 = *(aes_encrypt_addr + 1);
    unsigned cipher2 = *(aes_encrypt_addr + 2);
    unsigned cipher3 = *(aes_encrypt_addr + 3);

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
void aesDecrypt(unsigned char key[], unsigned char ciphertext[], unsigned char plaintext[], int keyexp)
{
    unsigned word4 = (ciphertext[12] << 24) + (ciphertext[13] << 16) + (ciphertext[14] << 8) + ciphertext[15];
    unsigned word5 = (ciphertext[8] << 24) + (ciphertext[9] << 16) + (ciphertext[10] << 8) + ciphertext[11];
    unsigned word6 = (ciphertext[4] << 24) + (ciphertext[5] << 16) + (ciphertext[6] << 8) + ciphertext[7];
    unsigned word7 = (ciphertext[0] << 24) + (ciphertext[1] << 16) + (ciphertext[2] << 8) + ciphertext[3];

    // Write plaintext to AES decryption module
    *(aes_decrypt_addr + 4) = word4;
    *(aes_decrypt_addr + 5) = word5;
    *(aes_decrypt_addr + 6) = word6;
    *(aes_decrypt_addr + 7) = word7;

    if (keyexp)
    {
        // Input key and perform key expansion
        unsigned word0 = (key[12] << 24) + (key[13] << 16) + (key[14] << 8) + key[15];
        unsigned word1 = (key[8] << 24) + (key[9] << 16) + (key[10] << 8) + key[11];
        unsigned word2 = (key[4] << 24) + (key[5] << 16) + (key[6] << 8) + key[7];
        unsigned word3 = (key[0] << 24) + (key[1] << 16) + (key[2] << 8) + key[3];

        // Write key to AES decryption module
        *(aes_decrypt_addr + 0) = word0;
        *(aes_decrypt_addr + 1) = word1;
        *(aes_decrypt_addr + 2) = word2;
        *(aes_decrypt_addr + 3) = word3;

        *(aes_decrypt_addr + 8) = (unsigned)0;
    }
    else
    {
        // Don't perform key expansion
        *(aes_decrypt_addr + 9) = (unsigned)0;
    }

    unsigned plain0 = *(aes_decrypt_addr + 0);
    unsigned plain1 = *(aes_decrypt_addr + 1);
    unsigned plain2 = *(aes_decrypt_addr + 2);
    unsigned plain3 = *(aes_decrypt_addr + 3);

    for (int i = 0; i < 4; i++)
    {
        plaintext[i] = plain3 >> (8 * (3 - i));
        plaintext[i + 4] = plain2 >> (8 * (3 - i));
        plaintext[i + 8] = plain1 >> (8 * (3 - i));
        plaintext[i + 12] = plain0 >> (8 * (3 - i));
    }
}

/**
 * Testing whether encryption and decryption modules work as expected
 */
// int main()
// {
//     int debug = 1;
//     int wrong = 0;
//     unsigned char key[] = {0x8a, 0x31, 0x47, 0xfa, 0xb7, 0xd3, 0x65, 0x1d, 0x74, 0xd9, 0x0a, 0x11, 0x17, 0x53, 0x66, 0xd4};
//     unsigned char plaintext[] = {0x65, 0x5d, 0x62, 0x0f, 0x52, 0x58, 0x6f, 0x28, 0x56, 0x06, 0x15, 0x72, 0x69, 0x61, 0x57, 0x1b};
//     unsigned char ciphertext[16];
//     unsigned char ref_ciphertext[] = {0x9d, 0x42, 0xf2, 0x36, 0x12, 0xa5, 0x82, 0x23, 0x56, 0x6e, 0x30, 0x0f, 0xf4, 0xf7, 0x48, 0x0a};

//     aes_encrypt(key, plaintext, ciphertext, 1);
//     // Correct solution for the above cipher blocks
//     // 	f4f7480a
//     // 	566e300f
//     //	12a58223
//     //	9d42f236
//     if (debug)
//     {
//         for (int i = 0; i < 16; i++)
//         {
//             printf("%x ", ciphertext[i]);
//         }
//         printf("\n");
//     }

//     int correct = 1;
//     for (int i = 0; i < 16; i++)
//     {
//         if (ciphertext[i] != ref_ciphertext[i])
//         {
//             correct = 0;
//         }
//     }
//     if (correct)
//     {
//         printf("KEY EXPANSION: Encryption worked!\n");
//     }
//     else
//     {
//         wrong++;
//         printf("KEY EXPANSION: Encryption did NOT work!!!!!\n");
//     }

//     unsigned char plaintext1[16];
//     aes_decrypt(key, ciphertext, plaintext1, 1);
//     if (debug)
//     {
//         for (int i = 0; i < 16; i++)
//         {
//             printf("%x ", plaintext1[i]);
//         }
//         printf("\n");
//     }

//     correct = 1;
//     for (int i = 0; i < 16; i++)
//     {
//         if (plaintext[i] != plaintext1[i])
//         {
//             correct = 0;
//         }
//     }
//     if (correct)
//     {
//         printf("KEY EXPANSION: Decryption worked!\n");
//     }
//     else
//     {
//         wrong++;
//         printf("KEY EXPANSION: Decryption did NOT work!!!!!\n");
//     }

//     /* ================================================================================================================================
//      * Starting no key expansion test
//      */
//     // Purposely creating invalid key to test whether function uses this key (it shouldn't)
//     unsigned char key2[] = {0x0};
//     unsigned char plaintext2[] = {0x2a, 0x1f, 0x22, 0x16, 0x00, 0x12, 0x1c, 0x7c, 0x75, 0x34, 0x40, 0x41, 0x1e, 0x1b, 0x12, 0x4a};
//     unsigned char ref_ciphertext2[] = {0x4e, 0xcf, 0x34, 0x08, 0xf0, 0x46, 0x29, 0xd8, 0xa1, 0x28, 0x9c, 0x12, 0xc8, 0xef, 0xb3, 0x5e};
//     aes_encrypt(key2, plaintext2, ciphertext, 0);
//     // Correct solution for the above cipher blocks
//     // 	c8efb35e
//     // 	a1289c12
//     // 	f04629d8
//     // 	4ecf3408
//     if (debug)
//     {
//         for (int i = 0; i < 16; i++)
//         {
//             printf("%x ", ciphertext[i]);
//         }
//         printf("\n");
//     }

//     correct = 1;
//     for (int i = 0; i < 16; i++)
//     {
//         if (ciphertext[i] != ref_ciphertext2[i])
//         {
//             correct = 0;
//         }
//     }
//     if (correct)
//     {
//         printf("NO KEY EXPANSION: Encryption worked!\n");
//     }
//     else
//     {
//         wrong++;
//         printf("NO KEY EXPANSION: Encryption did NOT work!!!!!\n");
//     }

//     unsigned char plaintext3[16];
//     aes_decrypt(key, ciphertext, plaintext3, 0);
//     if (debug)
//     {
//         for (int i = 0; i < 16; i++)
//         {
//             printf("%x ", plaintext3[i]);
//         }
//         printf("\n");
//     }

//     correct = 1;
//     for (int i = 0; i < 16; i++)
//     {
//         if (plaintext2[i] != plaintext3[i])
//         {
//             correct = 0;
//         }
//     }

//     if (correct)
//     {
//         printf("NO KEY EXPANSION: Decryption worked!\n");
//     }
//     else
//     {
//         wrong++;
//         printf("NO KEY EXPANSION: Decryption did NOT work!!!!!\n");
//     }

//     printf("\n");
//     if (wrong)
//     {
//         printf("Failed %d tests...\n", wrong);
//     }
//     else
//     {
//         printf("Passed all tests!\n");
//     }

//     return 0;
// }

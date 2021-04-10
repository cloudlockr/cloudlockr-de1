/**
 * This module contains several tests for the firmware application
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "memAddress.h"
#include "aesHwAcc.h"
#include "verificationService.h"
#include "hexService.h"

/**
 * Test 0 for whether encryption and decryption modules work as expected.
 * Encrypting and decrypting with known key and comparing with reference solution.
 */
void aes_test0()
{
    int debug = 0;
    int wrong = 0;
    unsigned char key[] = {0x8a, 0x31, 0x47, 0xfa, 0xb7, 0xd3, 0x65, 0x1d, 0x74, 0xd9, 0x0a, 0x11, 0x17, 0x53, 0x66, 0xd4};
    unsigned char plaintext[] = {0x65, 0x5d, 0x62, 0x0f, 0x52, 0x58, 0x6f, 0x28, 0x56, 0x06, 0x15, 0x72, 0x69, 0x61, 0x57, 0x1b};
    unsigned char ciphertext[16];
    unsigned char ref_ciphertext[] = {0x9d, 0x42, 0xf2, 0x36, 0x12, 0xa5, 0x82, 0x23, 0x56, 0x6e, 0x30, 0x0f, 0xf4, 0xf7, 0x48, 0x0a};

    encrypt(key, plaintext, ciphertext, 1);
    if (debug)
    {
        for (int i = 0; i < 16; i++)
        {
            printf("%x ", ciphertext[i]);
        }
        printf("\n");
    }

    int correct = 1;
    for (int i = 0; i < 16; i++)
    {
        if (ciphertext[i] != ref_ciphertext[i])
        {
            correct = 0;
        }
    }
    if (correct)
    {
        printf("KEY EXPANSION: Encryption worked!\n");
    }
    else
    {
        wrong++;
        printf("KEY EXPANSION: Encryption did NOT work!!!!!\n");
    }

    unsigned char plaintext1[16];
    decrypt(key, ciphertext, plaintext1, 1);
    if (debug)
    {
        for (int i = 0; i < 16; i++)
        {
            printf("%x ", plaintext1[i]);
        }
        printf("\n");
    }

    correct = 1;
    for (int i = 0; i < 16; i++)
    {
        if (plaintext[i] != plaintext1[i])
        {
            correct = 0;
        }
    }
    if (correct)
    {
        printf("KEY EXPANSION: Decryption worked!\n");
    }
    else
    {
        wrong++;
        printf("KEY EXPANSION: Decryption did NOT work!!!!!\n");
    }

    /* ================================================================================================================================
     * Starting no key expansion test
     */
    // Purposely creating invalid key to test whether function uses this key (it shouldn't)
    unsigned char key2[] = {0x0};
    unsigned char plaintext2[] = {0x2a, 0x1f, 0x22, 0x16, 0x00, 0x12, 0x1c, 0x7c, 0x75, 0x34, 0x40, 0x41, 0x1e, 0x1b, 0x12, 0x4a};
    unsigned char ref_ciphertext2[] = {0x4e, 0xcf, 0x34, 0x08, 0xf0, 0x46, 0x29, 0xd8, 0xa1, 0x28, 0x9c, 0x12, 0xc8, 0xef, 0xb3, 0x5e};
    encrypt(key2, plaintext2, ciphertext, 0);
    // Correct solution for the above cipher blocks
    // 	c8efb35e
    // 	a1289c12
    // 	f04629d8
    // 	4ecf3408
    if (debug)
    {
        for (int i = 0; i < 16; i++)
        {
            printf("%x ", ciphertext[i]);
        }
        printf("\n");
    }

    correct = 1;
    for (int i = 0; i < 16; i++)
    {
        if (ciphertext[i] != ref_ciphertext2[i])
        {
            correct = 0;
        }
    }
    if (correct)
    {
        printf("NO KEY EXPANSION: Encryption worked!\n");
    }
    else
    {
        wrong++;
        printf("NO KEY EXPANSION: Encryption did NOT work!!!!!\n");
    }

    unsigned char plaintext3[16];
    decrypt(key, ciphertext, plaintext3, 0);
    if (debug)
    {
        for (int i = 0; i < 16; i++)
        {
            printf("%x ", plaintext3[i]);
        }
        printf("\n");
    }

    correct = 1;
    for (int i = 0; i < 16; i++)
    {
        if (plaintext2[i] != plaintext3[i])
        {
            correct = 0;
        }
    }

    if (correct)
    {
        printf("NO KEY EXPANSION: Decryption worked!\n");
    }
    else
    {
        wrong++;
        printf("NO KEY EXPANSION: Decryption did NOT work!!!!!\n");
    }

    printf("\n");
    if (wrong)
    {
        printf("Failed AES test 0\n", wrong);
    }
    else
    {
        printf("Passed AES test 0\n");
    }
}

/**
 * Test 1 for whether encryption and decryption modules work as expected.
 * Encrypting and decrypting with random keys, plaintexts, and ciphertexts.
 * Mainly ensuring that decrypted text matches original plaintext.
 */
void aes_test1()
{
    time_t t;
    srand((unsigned)time(&t));
    int correct = 1;

    unsigned char key[16], plaintext[16], ciphertext[16], plaintext1[16];

    // Verify 10,000 times
    for (int i = 0; i < 10000; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            key[j] = (unsigned char)rand() % 256;
            plaintext[j] = (unsigned char)rand() % 256;
        }

        // Encrypt then immediately decrypt, and only do key expansion the first time
        encrypt(key, plaintext, ciphertext, i == 0);
        decrypt(key, ciphertext, plaintext1, i == 0);

        for (int j = 0; j < 16; j++)
        {
            // If the decrypted text does not match the original plaintext, test fails
            if (plaintext[j] != plaintext1[j])
            {
                correct = 0;
                break;
            }
        }

        if (!correct)
            break;
    }

    if (!correct)
    {
        printf("Failed AES test 1\n");
    }
    else
    {
        printf("Passed AES test 1\n");
    }
}

/**
 * Test for getting, setting, and verifying master password
 */
void password_test()
{
    int correct = 1;
    char *password0 = "firstpassword";
    int password0_length = strlen(password0) + 1;
    set_password(password0);

    char *got_password0 = (char *)malloc(sizeof(char) * password0_length);
    get_password(got_password0);

    for (int i = 0; i < password0_length; i++)
    {
        if (password0[i] != got_password0[i])
        {
            correct = 0;
        }
    }

    char *password1 = "09jw8f923hr982dsfiwe34j4";
    int password1_length = strlen(password1) + 1;
    set_password(password1);

    char *got_password1 = (char *)malloc(sizeof(char) * password1_length);
    get_password(got_password1);

    for (int i = 0; i < password1_length; i++)
    {
        if (password1[i] != got_password1[i])
        {
            correct = 0;
        }
    }

    if (!correct)
    {
        printf("Failed password test part 1\n");
    }
    else
    {
        printf("Passed password test part 1\n");
    }

    correct = 0;
    for (int i = 0; i < password1_length; i++)
    {
        if (password0[i] != got_password1[i])
        {
            correct = 1;
        }
    }
    free(got_password1);
    free(got_password0);

    if (!correct)
    {
        printf("Failed password test part 2\n");
    }
    else
    {
        printf("Passed password test part 2\n");
    }
}

/**
 * Test for setting HEX display, everytime tests are ran the HEX display should change
 */
void hex_test()
{
    generate_display_hex_code();
}

/**
 * Uncomment the following main function and comment out the cloudlockrMain.c main function
 * to run the tests
 */
// int main()
// {
//     aes_test0();
//     aes_test1();
//     password_test();
//     hex_test();
// }

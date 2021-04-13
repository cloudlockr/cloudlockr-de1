/**
 * This module contains several tests for the firmware application
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "constants.h"
#include "memAddress.h"
#include "aesHwacc.h"
#include "verificationService.h"
#include "jsonParser.h"
#include "hexService.h"
#include "bluetoothService.h"
#include "wifiService.h"
#include "processingService.h"

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
        printf("Failed AES test 0\n");
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
 * Integration tests for each message type.
 */
/**
 * Message type 1, generateDisplayHexCode()
 */
void message1_test1() {
    generate_display_hex_code();
    printf("Check if HEX has generated new numbers to pass message 1 test 1\n");
}

/**
 * Message type 2, verify()
 * password and hex correct = true
 */
void message2_test1() {
	set_password("sending rending lending blending");
	*HEX_ADDR = (unsigned)0x123ABC;
    int success = verify("sending rending lending blending", "123ABC");
    if (success)
    {
    	printf("Passed message 2 test 1\n");
    }
    else
    {
    	printf("Failed message 2 test 1\n");
    }
}

/**
 * Message type 2, verify()
 * password wrong and hex correct = fail
 */
void message2_test2() {
	set_password("sending lending blending");
	*HEX_ADDR = (unsigned)0xABC123;
    int success = verify("sending rending lending blending", "ABC123");
    if (!success)
    {
    	printf("Passed message 2 test 2\n");
    }
    else
    {
    	printf("Failed message 2 test 2\n");
    }
}

/**
 * Message type 2, verify()
 * password correct and hex wrong = fail
 */
void message2_test3() {
	set_password("rending lending blending");
	*HEX_ADDR = (unsigned)0x123ABC;
    int success = verify("rending lending blending", "AB12BC");
    if (!success)
    {
    	printf("Passed message 2 test 3\n");
    }
    else
    {
    	printf("Failed message 2 test 3\n");
    }
}

/**
 * Message type 2, verify()
 * password and hex correct = pass
 */
void message2_test4() {
	set_password("rending lending blending");
	*HEX_ADDR = (unsigned)0xAB12BC;
    int success = verify("rending lending blending", "AB12BC");
    if (success)
    {
    	printf("Passed message 2 test 4\n");
    }
    else
    {
    	printf("Failed message 2 test 4\n");
    }
}

/**
 * Message type 2, verify()
 * password and hex wrong = fail
 */
void message2_test5() {
	set_password("rending blending");
	*HEX_ADDR = (unsigned)0xEF01234;
    int success = verify("rending lending blending", "AB12BC");
    if (!success)
    {
    	printf("Passed message 2 test 5\n");
    }
    else
    {
    	printf("Failed message 2 test 5\n");
    }
}

/**
 * Message type 2, verify()
 * password and hex correct = pass
 * with json str
 */
void message2_test6() {
	set_password("1234567890abc");
	*HEX_ADDR = (unsigned)0xABCDEF;
	char *json_str = "{\"type\":2,\"password\":\"1234567890abc\",\"hex\":\"ABCDEF\"}";
    jsmntok_t *json_tokens = str_to_json(json_str);

	// Check for JSON parsing errors
	if (json_tokens == NULL)
	{
    	printf("Failed message 2 test 6\n");
    	return;
	}

	int expected_num_values = 3;
    char **all_values;
    all_values = get_json_values(json_str, json_tokens, expected_num_values);
    int success = verify(all_values[1], all_values[2]);
    if (success)
    {
    	printf("Passed message 2 test 6\n");
    }
    else
    {
    	printf("Failed message 2 test 6\n");
    }
}

/**
 * Message type 2, verify()
 * password wrong and hex correct = fail
 * with json str
 */
void message2_test7() {
	set_password("mosh1prove5zsd");
	*HEX_ADDR = (unsigned)0xABCDEF;
	char *json_str = "{\"type\":2,\"password\":\"1234567890abc\",\"hex\":\"ABCDEF\"}";
    jsmntok_t *json_tokens = str_to_json(json_str);

	// Check for JSON parsing errors
	if (json_tokens == NULL)
	{
    	printf("Failed message 2 test 7\n");
    	return;
	}

	int expected_num_values = 3;
    char **all_values;
    all_values = get_json_values(json_str, json_tokens, expected_num_values);
    int success = verify(all_values[1], all_values[2]);
    if (!success)
    {
    	printf("Passed message 2 test 7\n");
    }
    else
    {
    	printf("Failed message 2 test 7\n");
    }
}

/**
 * Message type 2, verify()
 * password correct and hex wrong = fail
 * with json str
 */
void message2_test8() {
	set_password("1234567890abc");
	*HEX_ADDR = (unsigned)0x012345;
	char *json_str = "{\"type\":2,\"password\":\"1234567890abc\",\"hex\":\"ABCDEF\"}";
    jsmntok_t *json_tokens = str_to_json(json_str);

	// Check for JSON parsing errors
	if (json_tokens == NULL)
	{
    	printf("Failed message 2 test 8\n");
    	return;
	}

	int expected_num_values = 3;
    char **all_values;
    all_values = get_json_values(json_str, json_tokens, expected_num_values);
    int success = verify(all_values[1], all_values[2]);
    if (!success)
    {
    	printf("Passed message 2 test 8\n");
    }
    else
    {
    	printf("Failed message 2 test 8\n");
    }
}

/**
 * Message type 2, verify()
 * password and hex wrong = fail
 * with json str
 */
void message2_test9() {
	set_password("jsonnnon1");
	*HEX_ADDR = (unsigned)0x987654;
	char *json_str = "{\"type\":2,\"password\":\"1234567890abc\",\"hex\":\"ABCDEF\"}";
    jsmntok_t *json_tokens = str_to_json(json_str);

	// Check for JSON parsing errors
	if (json_tokens == NULL)
	{
    	printf("Failed message 2 test 9\n");
    	return;
	}

	int expected_num_values = 3;
    char **all_values;
    all_values = get_json_values(json_str, json_tokens, expected_num_values);
    int success = verify(all_values[1], all_values[2]);
    if (!success)
    {
    	printf("Passed message 2 test 9\n");
    }
    else
    {
    	printf("Failed message 2 test 9\n");
    }
}


/**
 * Message type 3 and 4, upload() and download
 * please change to your wifi name and password to test.
 */
void message34_test1() {
	// Change Me!
	set_wifi_config("networkWrongName", "I4a3Tes90Eap3enN7es");

	extern int mock_idx;
	mock_idx = 4;
	char *json_str = bluetooth_wait_for_data();
	jsmntok_t *json_tokens = str_to_json(json_str);

	// Check for JSON parsing errors
	if (json_tokens == NULL)
	{
		printf("Failed message 3-4 test 1\n");
		return;
	}

	int expected_num_values = 6;
	char **all_values;
	all_values = get_json_values(json_str, json_tokens, expected_num_values);
	int packet_number = (int)strtol(all_values[2], NULL, 10);
	int total_packets = (int)strtol(all_values[3], NULL, 10);
	char *response_data = upload(all_values[1], packet_number, total_packets, all_values[4], all_values[5]);

	json_str = bluetooth_wait_for_data();
	json_tokens = str_to_json(json_str);

	// Check for JSON parsing errors
	if (json_tokens == NULL)
	{
		printf("Failed message 3, 4 test 1\n");
		return;
	}

	expected_num_values = 4;
	all_values = get_json_values(json_str, json_tokens, expected_num_values);
	download(all_values[2], all_values[1], all_values[3]);

}

/**
 * Message type 3, upload()
 * tests a good json
 *
 */
void message3_test1() {

	char *json_str = "{\"type\":3,\"fileId\":\"d869c9d6-1227-40ca-a3e8-bc11db68a1ab\",\"packetNumber\":1,\"totalPackets\":3,\"location\":\"37.422|-122.084|5.285\",\"fileData\":\"1234567890abcdeffedcba0987654321\"}";
	jsmntok_t *json_tokens = str_to_json(json_str);

	// Check for JSON parsing errors
	if (json_tokens == NULL)
	{
    	printf("Failed message 3 test 1\n");
    	return;
	}

    int expected_num_values = 6;
    char **all_values;
    all_values = get_json_values(json_str, json_tokens, expected_num_values);
    int packet_number = (int)strtol(all_values[2], NULL, 10);
    int total_packets = (int)strtol(all_values[3], NULL, 10);
    char *response_data = upload(all_values[1], packet_number, total_packets, all_values[4], all_values[5]); //TODO mock wifi here
    int success = 1;
    if (success)
    {
    	printf("Passed message 3 test 1\n");
    }
    else
    {
    	printf("Failed message 3 test 1\n");
    }
}

/**
 * Message type 4, download()
 *
 */
void message4_test1() {

	char *json_str = "{\"type\":4,\"localEncryptionComponent\":\"0102ABCD\",\"fileId\":\"d869c9d6-1227-40ca-a3e8-bc11db68a1ab\",\"location\":\"37.422|-122.084|5.285\"}";
	jsmntok_t *json_tokens = str_to_json(json_str);

	// Check for JSON parsing errors
	if (json_tokens == NULL)
	{
    	printf("Failed message 4 test 1\n");
    	return;
	}

	int expected_num_values = 4;
	char **all_values;
	all_values = get_json_values(json_str, json_tokens, expected_num_values);
	download(all_values[2], all_values[1], all_values[3]);
    int success = 1;
    if (!success)
    {
    	printf("Passed message 4 test 1\n");
    }
    else
    {
    	printf("Failed message 4 test 1\n");
    }
}

/**
 * Message type 6, set_wifi_config()
 * name password correct = pass
 *
 */
void message6_test1() {

	char *json_str = "{\"type\":6,\"networkName\":\"networkName\",\"networkPassword\":\"networkPassword\"}";
	jsmntok_t *json_tokens = str_to_json(json_str);

	// Check for JSON parsing errors
	if (json_tokens == NULL)
	{
    	printf("Failed message 6 test 1\n");
    	return;
	}

	int expected_num_values = 4;
	char **all_values;
	all_values = get_json_values(json_str, json_tokens, expected_num_values);

    int success = set_wifi_config(all_values[1], all_values[2]);
    if (success)
    {
    	printf("Passed message 6 test 1\n");
    }
    else
    {
    	printf("Failed message 6 test 1\n");
    }
}

/**
 * Message type 6, set_wifi_config()
 * name wrong password correct = fail
 *
 */
void message6_test2() {

	char *json_str = "{\"type\":6,\"networkName\":\"networkWrongName\",\"networkPassword\":\"networkPassword\"}";
	jsmntok_t *json_tokens = str_to_json(json_str);

	// Check for JSON parsing errors
	if (json_tokens == NULL)
	{
    	printf("Failed message 6 test 2\n");
    	return;
	}


	int expected_num_values = 4;
	char **all_values;
	all_values = get_json_values(json_str, json_tokens, expected_num_values);

    int success = set_wifi_config(all_values[1], all_values[2]);
    if (!success)
    {
    	printf("Passed message 6 test 2\n");
    }
    else
    {
    	printf("Failed message 6 test 2\n");
    }
}

/**
 * Message type 6, set_wifi_config()
 * name wrong password correct = fail
 *
 */
void message6_test3() {

	char *json_str = "{\"type\":6,\"networkName\":\"networkName\",\"networkPassword\":\"networkWrongPassword\"}";
	jsmntok_t *json_tokens = str_to_json(json_str);

	// Check for JSON parsing errors
	if (json_tokens == NULL)
	{
    	printf("Failed message 6 test 3\n");
    	return;
	}

	int expected_num_values = 4;
	char **all_values;
	all_values = get_json_values(json_str, json_tokens, expected_num_values);

    int success = set_wifi_config(all_values[1], all_values[2]);
    if (!success)
    {
    	printf("Passed message 6 test 3\n");
    }
    else
    {
    	printf("Failed message 6 test 3\n");
    }
}

/**
 * Message type 6, set_wifi_config()
 * name wrong password correct = fail
 *
 */
void message6_test4() {

	char *json_str = "{\"type\":6,\"networkName\":\"networkWrongName\",\"networkPassword\":\"networkWrongPassword\"}";
	jsmntok_t *json_tokens = str_to_json(json_str);

	// Check for JSON parsing errors
	if (json_tokens == NULL)
	{
    	printf("Failed message 6 test 4\n");
    	return;
	}

	int expected_num_values = 4;
	char **all_values;
	all_values = get_json_values(json_str, json_tokens, expected_num_values);

    int success = set_wifi_config(all_values[1], all_values[2]);
    if (!success)
    {
    	printf("Passed message 6 test 4\n");
    }
    else
    {
    	printf("Failed message 6 test 4\n");
    }
}

/**
 * Message type 7, set_password()
 * check if value goes to memory
 *
 */
void message7_test1() {

	char *json_str = "{\"type\":7,\"password\":\"1234567890abc\"}";
	jsmntok_t *json_tokens = str_to_json(json_str);

	// Check for JSON parsing errors
	if (json_tokens == NULL)
	{
    	printf("Failed message 7 test 1\n");
    	return;
	}

	int expected_num_values = 2;
	char **all_values;
    all_values = get_json_values(json_str, json_tokens, expected_num_values);

    set_password(all_values[1]);

    int success = 1;
    char *cmp_str = "1234567890abc";
    for (int i = 0; i < MAX_PASSWORD_LENGTH; i++) {
    	if (*(MASTER_PW_ADDR + i) != *(cmp_str)) {
    		success = 0;
    		break;
    	}
    	if (*(MASTER_PW_ADDR + i) == '\0') break;
    }
    if (!success)
    {
    	printf("Passed message 7 test 1\n");
    }
    else
    {
    	printf("Failed message 7 test 1\n");
    }
}

/**
 * Uncomment the following main function and comment out the cloudlockrMain.c main function
 * to run the tests
 */
//  int main()
//  {
//      aes_test0();
//      aes_test1();
//      password_test();
//      //hex_test();
//      message1_test1();

//      message2_test1();
//      message2_test2();
//      message2_test3();
//      message2_test4();
//      message2_test5();
//      message2_test6();
//      message2_test7();
//      message2_test8();
//      message2_test9();

//      message7_test1();
//  }

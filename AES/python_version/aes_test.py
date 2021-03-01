"""
This module tests the correctness of the python AES implementation by generating random keys
and random data to encrypt and decrypt. If all data match after encrypting and decrypting, then
the test is passed
"""

import random
from aes import AES


try:
    num_blocks = 15
    # Generate 10000 keys and data to encrypt and decrypt
    for _ in range(1000):
        private_key = ""
        plaintext = ""
        #KEY
        for _ in range(16):
            key_digit = hex(random.randint(0, 255))[2:]
            if len(key_digit) == 1:
                key_digit = "0" + key_digit
            private_key += key_digit

        # PLAINTEXT
        for _ in range(num_blocks * 16):
            text_digit = hex(random.randint(0, 127))[2:]
            if len(text_digit) == 1:
                text_digit = "0" + text_digit
            plaintext += text_digit

        aes = AES(private_key)
        ciphertext = aes.encrypt(plaintext)
        decrypted_text = aes.decrypt(ciphertext)

        if plaintext != decrypted_text:
            # Log data if there is a mismatch
            print("NOT MATCHING")
            print("Key: ", key)
            print("PLaintext: ", plaintext)
            print("Length of plaintext", len(plaintext))
            print()
            print(decrypted_text)
            exit()
except Exception:
    # Log data if there is an exception
    print("Met exception!")
    print("Key: ", key)
    print("PLaintext: ", plaintext)
    print("Length of plaintext", len(plaintext))
    print()
    print(decrypted_text)
    exit()

print("PASSED")

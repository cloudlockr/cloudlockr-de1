"""
This module tests the correctness of the python AES implementation by generating random keys
and random data to encrypt and decrypt. If all data match after encrypting and decrypting, then
the test is passed
"""

import random
import string
from aes import AES


letters0 = string.ascii_letters
letters1 = string.digits
letters2 = string.punctuation
letters = letters0 + letters1 + letters2

try:
    # Generate 10000 keys and data to encrypt and decrypt
    for _ in range(1000):
        key = random.randint(2**127, 2**128-1)
        key = key.to_bytes(16, byteorder="big")
        plaintext = bytes("".join(random.choice(letters) for i in range(random.randint(20, 300))), encoding="ascii")

        aes = AES(key)
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

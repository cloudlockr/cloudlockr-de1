import random
import string
from aes import AES


letters0 = string.ascii_letters
letters1 = string.digits
letters2 = string.punctuation
letters = letters0 + letters1 + letters2

for _ in range(10000):
    key = "".join(random.choice(letters1) for i in range(16))
    plaintext = "".join(random.choice(letters) for i in range(500))

    aes = AES(key)
    ciphertext = aes.encrypt(plaintext)
    decrypted_text = aes.decrypt(ciphertext)

    if plaintext != decrypted_text:
        print("NOT MATCHING")
        print(plaintext)
        print()
        print(decrypted_text)
        exit()

print("PASSED")

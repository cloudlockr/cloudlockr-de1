"""
This module is an implementation of the AES algorithm in python in
preparation for the implementation in verilog.
This implementation will be used to verify the correctness of the version written
in verilog.
"""

import numpy as np
import random
from sbox import SBOX, REVERSE_SBOX


# The AES class only accepts bytes as the secret input key and encryption/decryption data
class AES:
    def __init__(self, key):
        self.rounds = 10

        # helper functions to efficiently apply function to numpy arrays element wise
        self.sub_bytes = np.vectorize(lambda x : SBOX[x//16, x%16])
        self.reverse_sub_bytes = np.vectorize(lambda x : REVERSE_SBOX[x//16, x%16])
        self.vmult = np.vectorize(self.mult2)

        # essential elements for AES algorithm
        self.key = np.zeros((self.rounds + 1, 4, 4), dtype=np.uint8)
        self.key[0] = self.turn_text_to_mat(key)[0]
        self.key_expansion()

    def key_expansion(self):
        rcon = np.array([1, 0, 0, 0], dtype=np.uint8)

        for i in range(1, self.rounds + 1):
            prev_key = self.key[i-1]
            shifted = np.roll(prev_key.T[3], -1)
            subbed = self.sub_bytes(shifted)
            self.key[i].T[0] = prev_key.T[0] ^ subbed ^ rcon

            for j in range(1, 4):
                self.key[i].T[j] = prev_key.T[j] ^ self.key[i].T[j-1]

            if rcon[0] < 80:
                rcon[0] = rcon[0] * 2
            else:
                rcon[0] = (rcon[0] * 2) ^ 0x11b

    @staticmethod
    def add_padding(text):
        # If text is not multiple of 32 bytes, need to pad it
        pad_bytes = 32 - len(text) % 32
        if pad_bytes % 32 == 0:
            return text

        padding = bytes(" ", encoding="ascii")
        for _ in range(pad_bytes):
            text += padding
        return text

    @staticmethod
    def remove_padding(text):
        to_keep = len(text)
        for byte in reversed(text):
            if byte == ord(" "):
                to_keep -= 1
            else:
                break
        
        return text[:to_keep]

    def turn_text_to_mat(self, text):
        padded_text = self.add_padding(text)
        num_blocks = len(padded_text) // 32

        # Both text and key are represented with 4x4 matrix in AES, construct matrix grid
        blocks = np.zeros((num_blocks, 4, 4), dtype=np.uint8)
        for num in range(num_blocks):
            for col_i in range(4):
                for row_i in range(4):
                    digit = int(padded_text[num*32+col_i*8+row_i*2:num*32+col_i*8+row_i*2+2], 16)
                    blocks[num, row_i, col_i] = np.uint8(digit)

        return blocks

    def turn_mat_to_text(self, mat, remove_pad=True):
        # Turn matrix grid back to text
        text = ""
        for num in range(mat.shape[0]):
            for col_index in range(4):
                for row_index in range(4):
                    digit = hex(mat[num, row_index, col_index])[2:]
                    if len(digit) == 1:
                        digit = "0" + digit
                    text += digit

        # if remove_pad:
        #     text = self.remove_padding(text)
        return text

    @staticmethod
    def shift_rows(block):
        for row in range(1, 4):
            block[row] = np.roll(block[row], -row)

    @staticmethod
    def reverse_shift_rows(block):
        for row in range(1, 4):
            block[row] = np.roll(block[row], row)

    @staticmethod
    def mult2(num):
        # helper function to modular multiply
        num = np.uint8(num)
        original_num = num
        num = num << np.uint8(1)
        if num < original_num:
            num ^= np.uint8(0x1b)
        return num

    def mix_columns(self, block):
        block_copy = block.copy()
        mult_copy = self.vmult(block)
        for i, col in enumerate(block.T):
            col[0] = mult_copy[0, i] ^ mult_copy[1, i] ^ block_copy[1, i] ^ block_copy[2, i] ^ block_copy[3, i]
            col[1] = mult_copy[1, i] ^ mult_copy[2, i] ^ block_copy[2, i] ^ block_copy[3, i] ^ block_copy[0, i]
            col[2] = mult_copy[2, i] ^ mult_copy[3, i] ^ block_copy[3, i] ^ block_copy[0, i] ^ block_copy[1, i]
            col[3] = mult_copy[3, i] ^ mult_copy[0, i] ^ block_copy[0, i] ^ block_copy[1, i] ^ block_copy[2, i]

    def round_func(self, block, round_num):
        # Substitute bytes
        block = self.sub_bytes(block)

        # Shift rows
        self.shift_rows(block)

        # Mix columns
        # test_block = np.array([
        #     [219, 242, 212, 45],
        #     [19, 10, 212, 38],
        #     [83, 34, 212, 49],
        #     [69, 92, 213, 76]
        # ])
        if round_num < self.rounds:
            self.mix_columns(block)

        # add round key
        block = np.bitwise_xor(block, self.key[round_num])

        return block

    def reverse_round_func(self, block, round_num):
        # add round key
        block = np.bitwise_xor(block, self.key[round_num])

        # Mix columns
        if round_num < self.rounds:
            self.mix_columns(block)
            self.mix_columns(block)
            self.mix_columns(block)

        # Shift rows
        self.reverse_shift_rows(block)

        # Substitute bytes
        block = self.reverse_sub_bytes(block)

        return block

    def encrypt(self, plaintext):
        # Transform plaintext to 4x4 blocks
        plain_blocks = self.turn_text_to_mat(plaintext)

        cipher_blocks = np.zeros((len(plain_blocks), 4, 4), np.uint8)

        for i, plain_block in enumerate(plain_blocks):
            # First step: Add round key (XOR every bit of plaintext with key)
            cipher_block = np.bitwise_xor(plain_block, self.key[0])

            for round_num in range(1, self.rounds + 1):
                cipher_block = self.round_func(cipher_block, round_num)

            cipher_blocks[i] = cipher_block

        return self.turn_mat_to_text(cipher_blocks, remove_pad=False)

    def decrypt(self, ciphertext):
        # Transform ciphertext into 4x4 blocks
        cipher_blocks = self.turn_text_to_mat(ciphertext)
        plain_blocks = np.zeros((len(cipher_blocks), 4, 4), np.uint8)

        for i, cipher_block in enumerate(cipher_blocks):
            plain_block = cipher_block.copy()
            for round_num in range(self.rounds, 0, -1):
                plain_block = self.reverse_round_func(plain_block, round_num)

            plain_block = np.bitwise_xor(plain_block, self.key[0])
            plain_blocks[i] = plain_block

        return self.turn_mat_to_text(plain_blocks)

if __name__ == "__main__":
    private_key = ""
    plaintext = ""
    num_blocks = 1

    # #KEY
    # for _ in range(16):
    #     key_digit = hex(random.randint(0, 255))[2:]
    #     if len(key_digit) == 1:
    #         key_digit = "0" + key_digit
    #     private_key += key_digit
    
    # print(private_key)
    private_key = "d4665317110ad9741d65d3b7fa47318a"

    # PLAINTEXT
    for _ in range(num_blocks * 16):
        text_digit = hex(random.randint(0, 127))[2:]
        if len(text_digit) == 1:
            text_digit = "0" + text_digit
        plaintext += text_digit

    # Generate mem file for modelsim to load (encrypt)
    with open("../verilog_version/mem_content0.memh", "w") as f:
        # SBOX
        for row in range(16):
            for col in range(16):
                f.write(f"{hex(SBOX[row, col])[2:]}\n")
        
        # KEY
        for i in range(0, 16 * 2, 2):
            key_digit = private_key[i:i+2]
            f.write(f"{key_digit}\n")

        # PLAINTEXT
        for i in range(0, num_blocks * 16 * 2, 2):
            text_digit = plaintext[i:i+2]
            f.write(f"{text_digit}\n")

    # Generate mem file for reference (encrypt)
    with open("../verilog_version/ref_content1.memh", "w") as f:
        for i in range(0, num_blocks * 16 * 2, 2):
            text_digit = plaintext[i:i+2]
            f.write(f"{text_digit}\n")

    aes = AES(private_key)
    ciphertext = aes.encrypt(plaintext)

    # Generate mem file for modelsim to load (decrypt)
    with open("../verilog_version/mem_content1.memh", "w") as f:
        # SBOX
        for row in range(16):
            for col in range(16):
                f.write(f"{hex(SBOX[row, col])[2:]}\n")

        # REVERSE_SBOX
        for row in range(16):
            for col in range(16):
                f.write(f"{hex(REVERSE_SBOX[row, col])[2:]}\n")
        
        # KEY
        for i in range(0, 16 * 2, 2):
            key_digit = private_key[i:i+2]
            f.write(f"{key_digit}\n")

        # CIPHERTEXT
        for i in range(0, num_blocks * 16 * 2, 2):
            cipher_digit = ciphertext[i:i+2]
            f.write(f"{cipher_digit}\n")

    # Generate mem file for reference (decypt)
    with open("../verilog_version/ref_content0.memh", "w") as f:
        for i in range(0, num_blocks * 16 * 2, 2):
            cipher_digit = ciphertext[i:i+2]
            f.write(f"{cipher_digit}\n")
        print("Generated reference solution")

    decrypted_text = aes.decrypt(ciphertext)
    if plaintext == decrypted_text:
        print("Encryption and decryption worked")
    else:
        print("Encryption and decryption failed")
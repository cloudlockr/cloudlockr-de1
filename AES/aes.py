"""
This module is a test implementation of the AES algorithm in python in
preparation for the implementation in verilog.
"""
import numpy as np


class AES:
    def __init__(self, key):
        self.key = self.turn_text_to_mat(key)[0]

    def add_padding(self, text):
        # If text is not multiple of 16 bytes, need to pad it
        pad_bytes = 16 - len(text) % 16
        if pad_bytes % 16 == 0:
            return text

        for _ in range(pad_bytes):
            text += str(pad_bytes)
        return text

    def remove_padding(self, text):
        try:
            pad_bytes = int(text[-1])
            # Confirm that last byte was a padding
            if int(text[-pad_bytes]) == pad_bytes:
                return text[:-pad_bytes]

            return text
        except ValueError:
            return text

    def turn_text_to_mat(self, text):
        padded_text = self.add_padding(text)
        num_blocks = len(padded_text) // 16

        # Both text and key are represented with 4x4 matrix in AES, construct matrix grid
        blocks = np.zeros((num_blocks, 4, 4), dtype=int)
        for num in range(num_blocks):
            for col_index in range(4):
                for row_index in range(4):
                    blocks[num, row_index, col_index] = ord(padded_text[num * 16 + col_index * 4 + row_index])

        return blocks

    def turn_mat_to_text(self, mat):
        # Turn matrix grid back to text
        padded_text = ""
        for num in range(mat.shape[0]):
            for col_index in range(4):
                for row_index in range(4):
                    padded_text += chr(mat[num, row_index, col_index])

        text = self.remove_padding(padded_text)
        return text

    def encrypt(self, plaintext):
        # Transform plaintext to 4x4 blocks
        plain_blocks = self.turn_text_to_mat(plaintext)

        for plain_block in plain_blocks:
            # First step: Add round key (XOR every bit of plaintext with key)
            cipher_block = np.bitwise_xor(plain_block, self.key)


    def decrypt(self, ciphertext):
        raise NotImplementedError

if __name__ == "__main__":
    aes = AES("Random key 12345")
    aes.encrypt("wowowow this is some really random text")
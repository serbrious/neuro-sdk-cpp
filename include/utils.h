#pragma once


#include <string>
#include <vector>

// Base64 encoding
const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


static inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(const std::string& data) {
    std::string ret;
    int i = 0, j = 0;
    unsigned char char_array_3[3], char_array_4[4];

    for (size_t len = data.size(); i < len;) {
        char_array_3[i % 3] = data[i++];
        if (i % 3 == 0) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (size_t k = 0; k < 4; k++)
                ret += base64_chars[char_array_4[k]];
        }
    }

    if (i % 3) {
        for (j = i; j % 3; j++) char_array_3[j % 3] = 0;

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (size_t k = 0; k < i % 3 + 1; k++)
            ret += base64_chars[char_array_4[k]];

        while ((i++ % 3)) ret += '=';
    }

    return ret;
}

// SHA-1 hashing (simplified version, not secure for production)
static void sha1_process_block(const unsigned char block[64], uint32_t* h) {
    uint32_t w[80];
    for (int i = 0; i < 16; i++) {
        w[i] = (block[i * 4] << 24) | (block[i * 4 + 1] << 16) | (block[i * 4 + 2] << 8) | block[i * 4 + 3];
    }
    for (int i = 16; i < 80; i++) {
        w[i] = (w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16]);
        w[i] = (w[i] << 1) | (w[i] >> 31);
    }

    uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4];

    for (int i = 0; i < 80; i++) {
        uint32_t f, k;
        if (i < 20) {
            f = (b & c) | ((~b) & d);
            k = 0x5A827999;
        }
        else if (i < 40) {
            f = b ^ c ^ d;
            k = 0x6ED9EBA1;
        }
        else if (i < 60) {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8F1BBCDC;
        }
        else {
            f = b ^ c ^ d;
            k = 0xCA62C1D6;
        }

        uint32_t temp = ((a << 5) | (a >> 27)) + f + e + k + w[i];
        e = d;
        d = c;
        c = (b << 30) | (b >> 2);
        b = a;
        a = temp;
    }

    h[0] += a;
    h[1] += b;
    h[2] += c;
    h[3] += d;
    h[4] += e;
}

std::string sha1(const std::string& data) {
    uint32_t h[5] = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 };
    unsigned char block[64];
    size_t len = data.length();
    size_t chunk_len = ((len + 8) / 64 + 1) * 64;
    std::vector<unsigned char> padded(chunk_len);
    memcpy(padded.data(), data.c_str(), len);

    padded[len] = 0x80;
    for (size_t i = len + 1; i < chunk_len - 8; i++) padded[i] = 0;

    uint64_t bit_len = len * 8;
    for (int i = 0; i < 8; i++) {
        padded[chunk_len - 8 + i] = (bit_len >> (56 - 8 * i)) & 0xFF;
    }

    for (size_t i = 0; i < chunk_len; i += 64) {
        sha1_process_block(padded.data() + i, h);
    }

    std::string result;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 4; j++) {
            result.push_back((h[i] >> (24 - 8 * j)) & 0xFF);
        }
    }
    return base64_encode(result);
}
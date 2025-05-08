#include "cognit_encoding.h"
#include <stdio.h>

extern int my_base64_encode_cb(unsigned char str_b64_buff[], size_t buff_len, size_t *base64_len, char str[], int str_len);
extern int my_base64_decode_cb(char decoded_buff[], size_t buff_size, size_t* decoded_len, const unsigned char* str, size_t str_len);
extern int my_hash_cb(const unsigned char* str, size_t str_len, unsigned char hash[]);

int cognit_base64_encode(unsigned char str_b64_buff[], size_t buff_len, size_t *base64_len, char str[], int str_len)
{
    return my_base64_encode_cb(str_b64_buff, buff_len, base64_len, str, str_len);
}

int cognit_base64_decode(char decoded_buff[], size_t buff_size, size_t* decoded_len, const unsigned char* str, size_t str_len)
{
    return my_base64_decode_cb(decoded_buff, buff_size, decoded_len, str, str_len);
}

int cognit_hash(const unsigned char* str, size_t str_len, unsigned char hash[])
{
    return my_hash_cb(str, str_len, hash);
}
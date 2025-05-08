/*******************************************************/ /***
*	\file  ${file_name}
*	\brief HTTP client
*
*	Compiler  :  \n
*	Copyright : Samuel Pérez \n
*	Target    :  \n
*
*	\version $(date) ${user} $(remarks)
***********************************************************/
#ifndef COGNIT_ENCODING_H
#define COGNIT_ENCODING_H
/********************** INCLUDES **************************/
#include <stdint.h>
#include <stddef.h>
/***************** DEFINES AND MACROS *********************/

/**************** TYPEDEFS AND STRUCTS ********************/

/******************* GLOBAL VARIABLES *********************/

/******************* PUBLIC METHODS ***********************/

/*******************************************************/ /**
 * @brief Send HTTP request
 * 
 * @param buffer Buffer with the JSON payload
 * @param size Size of the buffer
 * @param config HTTP client configuration
 * @return int 0 if success, -1 otherwise
***********************************************************/
int cognit_base64_encode(unsigned char str_b64_buff[], size_t buff_len, size_t *base64_len, char str[], int str_len);

int cognit_base64_decode(char decoded_buff[], size_t buff_size, size_t* decoded_len, const unsigned char* str, size_t str_len);

int cognit_hash(const unsigned char* str, size_t str_len, unsigned char hash[]);

/******************* PRIVATE METHODS ***********************/

#endif // COGNIT_ENCODING_H
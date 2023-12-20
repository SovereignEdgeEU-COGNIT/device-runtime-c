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
#ifndef BASE64_H
#define BASE64_H
/********************** INCLUDES **************************/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
/***************** DEFINES AND MACROS *********************/


/**************** TYPEDEFS AND STRUCTS ********************/


/******************* GLOBAL VARIABLES *********************/
static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static char *decoding_table = NULL;
static int mod_table[] = {0, 2, 1};
/******************* PUBLIC METHODS ***********************/
char* base64_encode(const char* data, size_t input_length, size_t* output_length);

char* base64_decode(const char* data, size_t input_length, size_t* output_length);

/******************* PRIVATE METHODS ***********************/

#endif // BASE64_H
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
#ifndef _BASE64_H_
#define _BASE64_H_

#ifdef __cplusplus
extern "C" {
#endif

int base64_encode_len(int len);
int base64_encode(char* coded_dst, const char* plain_src, int len_plain_src);

int base64_decode_len(const char* coded_src);
int base64_decode(char* plain_dst, const char* coded_src);

#ifdef __cplusplus
}
#endif

#endif //_BASE64_H_
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
#ifndef FAAS_PARSER_H
#define FAAS_PARSER_H
/********************** INCLUDES **************************/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "pb_parser.h"

/***************** DEFINES AND MACROS *********************/
#define JSON_ERR_CODE_OK           0
#define JSON_ERR_CODE_INVALID_JSON -1
// Macro to include headers
#define INCLUDE_HEADERS(...) \
    (#__VA_ARGS__)


/**************** TYPEDEFS AND STRUCTS ********************/
typedef enum
{
    SUCCESS = 0,
    ERROR   = -1
} exec_return_code_t;

typedef struct SExecResponse
{
    exec_return_code_t ret_code;
    void** res;
    long err_code;
    size_t ret_len;
} exec_response_t;
/******************* PUBLIC METHODS ***********************/

/*******************************************************/ /**
 * @brief Parse the exec_faas_params_t struct to a JSON string
 * 
 * @param exec_faas_params Struct with the FaaS parameters
 * @param ui8_payload_buff Buffer to store the JSON string
 * @param payload_len Length of the JSON string
 * @return int8_t 0 if OK, -1 if error
***********************************************************/
int8_t faasparser_parse_exec_faas_params_as_str_json(faas_t* exec_faas_params, uint8_t* ui8_payload_buff, size_t* payload_len);

/*******************************************************/ /**
 * @brief Parse JSON string to exec_response_t struct
 * 
 * @param json_str JSON string
 * @param pt_res Pointer to store the response
 * @return int8_t 0 if OK, -1 if error
***********************************************************/
int8_t faasparser_parse_json_str_as_exec_response(const char* json_str, void** pt_res);

void faas_log_json_error_detail(const char* response_body);
/******************* PRIVATE METHODS ***********************/

#endif // FAAS_PARSER_H
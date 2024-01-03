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
#include <serverless_runtime_client.h>
/***************** DEFINES AND MACROS *********************/
#define JSON_ERR_CODE_OK           0
#define JSON_ERR_CODE_INVALID_JSON -1
// Macro to include headers
#define INCLUDE_HEADERS(...) \
    (#__VA_ARGS__)

// Macro to create a string from a function
#define FUNC_TO_STR(name, fn) \
    fn const char name##_str[] = #fn;

/**************** TYPEDEFS AND STRUCTS ********************/
typedef struct
{
    const char* type; // Float, int, char, bool
    const char* var_name;
    const char* value; // Coded b64
    char mode[4];      // "IN" or "OUT"
} param_t;

typedef struct
{
    char lang[2]; // "PY", "C"
    char* fc;
    param_t* params;
    size_t params_count;
} exec_faas_params_t;

/******************* GLOBAL VARIABLES *********************/

/******************* PUBLIC METHODS ***********************/
int8_t parse_exec_faas_params_as_str_json(exec_faas_params_t* exec_faas_params, uint8_t* ui8_payload_buff, size_t* payload_len);

int8_t parse_json_str_as_exec_response(const char* json_str, exec_response_t* t_exec_response);

int8_t parse_json_str_as_async_exec_response(const char* json_str, async_exec_response_t* t_async_exec_response);

/******************* PRIVATE METHODS ***********************/

#endif // FAAS_PARSER_H
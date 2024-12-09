/*******************************************************/ /***
*	\file  ${file_name}
*	\brief HTTP client
*
*	Compiler  :  \n
*	Copyright : Aitor Garciandia \n
*	Target    :  \n
*
*	\version $(date) ${user} $(remarks)
***********************************************************/
#ifndef CF_PARSER_H
#define CF_PARSER_H
/********************** INCLUDES **************************/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "cognit_frontend_cli.h"
/***************** DEFINES AND MACROS *********************/
#define JSON_ERR_CODE_OK           0
#define JSON_ERR_CODE_INVALID_JSON -1

/**************** TYPEDEFS AND STRUCTS ********************/
/******************* GLOBAL VARIABLES *********************/

/******************* PUBLIC METHODS ***********************/

/*******************************************************/ /**
 * @brief Parse the scheduling_t struct to a JSON string
 * 
 * @param t_app_requirements Struct with the app requirements configuration
 * @param ui8_payload_buff Buffer to store the JSON string
 * @param payload_len Length of the JSON string
 * @return int8_t 0 if OK, -1 if error
***********************************************************/
int8_t cfparser_parse_app_requirements_as_str_json(scheduling_t t_app_requirements, uint8_t* ui8_payload_buff, size_t* payload_len);

/*******************************************************/ /**
 * @brief Get app_req_id from JSON string
 * 
 * @param json_str JSON string
 * @param t_cognit_frontend Struct to store the response
 * @return int8_t 0 if OK, -1 if error
***********************************************************/
int8_t cfparser_parse_json_str_as_app_req_id(const char* json_str, cognit_frontend_cli_t* t_cognit_frontend);

/*******************************************************/ /**
 * @brief Get token from JSON string 
 * 
 * @param json_str JSON string
 * @param t_cognit_frontend Struct to store the response
 * @return int8_t 0 if OK, -1 if error
***********************************************************/
int8_t cfparser_parse_json_str_as_token(const char* json_str, cognit_frontend_cli_t* t_cognit_frontend);

/*******************************************************/ /**
 * @brief Get edge cluster frontend address from JSON string
 * 
 * @param json_str JSON string
 * @param t_ecf_response Struct to store the response
 * @return int8_t 0 if OK, -1 if error
***********************************************************/
int8_t cfparser_parse_json_str_as_ecf_address(const char* json_str, ecf_response_t* t_ecf_response);

/******************* PRIVATE METHODS ***********************/

#endif // CF_PARSER_H
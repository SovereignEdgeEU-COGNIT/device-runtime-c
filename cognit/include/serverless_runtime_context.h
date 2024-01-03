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
#ifndef SERVERLESS_RUNTIME_CONTEXT_H
#define SERVERLESS_RUNTIME_CONTEXT_H
/********************** INCLUDES **************************/
#include <stdio.h>
#include <stdbool.h>
#include <prov_engine_cli.h>
#include <serverless_runtime_client.h>
#include <faas_parser.h>
/***************** DEFINES AND MACROS *********************/
#define MODE_IN      "IN"
#define MODE_OUT     "OUT"
#define INTERVAL_1MS 1
/**************** TYPEDEFS AND STRUCTS ********************/

typedef enum
{
    E_ST_CODE_SUCCESS = 0,
    E_ST_CODE_ERROR   = 1,
    E_ST_CODE_PENDING = 2
} e_status_code_t;
typedef struct
{
    prov_eng_cli_context_t m_t_prov_eng_context;
    char m_c_endpoint[256];
    char m_c_url_proto[10];

} serverless_runtime_context_t;

/******************* GLOBAL VARIABLES *********************/

/******************* PUBLIC METHODS ***********************/

/*******************************************************/ /**
 * @brief Initialize the serverless runtime context and shares pec_context with prov_engine_cli
 * 
 * @param config Prov engine configuration
 * @return e_status_code_t Status returned by prov engine client
***********************************************************/
e_status_code_t init_serverless_runtime_context(cognit_config_t* config);

/*******************************************************/ /**
 * @brief Creates default serverless runtime configuration
 * 
 * @param t_basic_serverless_runtime_conf Basic serverless runtime configuration
 * @return e_status_code_t Status returned by prov engine client
***********************************************************/
e_status_code_t create_serverless_runtime(basic_serverless_runtime_conf_t t_basic_serverless_runtime_conf);

/*******************************************************/ /**
 * @brief Gets current serverless runtime status
 * 
 * @return const char* Serverless runtime status
***********************************************************/
const char* check_serverless_runtime_status();

/*******************************************************/ /**
 * @brief Parses faas_params, generates the payload and calls the sync serverless runtime
 * 
 * @param exec_faas_params Execution parameters
 * @return exec_response_t Execution response
***********************************************************/
exec_response_t call_sync(exec_faas_params_t* exec_faas_params);

/*******************************************************/ /**
 * @brief Parses faas_params, generates the payload and calls the async serverless runtime
 * 
 * @param exec_faas_params Execution parameters
 * @return async_exec_response_t Execution response
***********************************************************/
async_exec_response_t call_async(exec_faas_params_t* exec_faas_params);

/*******************************************************/ /**
 * @brief Ask periodically to the serverless runtime if the task has finished
 * 
 * @param c_async_task_id Task id to wait for
 * @param ui32_timeout_ms Timeout in milliseconds
 * @return async_exec_response_t Execution response
***********************************************************/
async_exec_response_t wait_for_task(const char* c_async_task_id, uint32_t ui32_timeout_ms);

/*******************************************************/ /**
 * @brief Ask pec to delete the serverless runtime
 * 
 * @param c_endpoint Serverless runtime endpoint
***********************************************************/
void delete_serverless_runtime(const char* c_endpoint);

/******************* PRIVATE METHODS ***********************/

#endif // SERVERLESS_RUNTIME_CONTEXT_H
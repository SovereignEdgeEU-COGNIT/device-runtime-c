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
/***************** DEFINES AND MACROS *********************/
#define MODE_IN  "IN"
#define MODE_OUT "OUT"

/**************** TYPEDEFS AND STRUCTS ********************/
typedef struct
{
    const char* type; // Float, int, char, bool
    const char* var_name;
    const char* value;  // Coded b64
    const char mode[4]; // "IN" or "OUT"
} param_t;

typedef struct
{
    char lang[3]; // "PY", "C"
    char* fc;
    param_t* params;
    size_t params_count;
} exec_faas_params_t;

typedef enum
{
    E_ST_CODE_SUCCESS = 0,
    E_ST_CODE_ERROR   = 1,
    E_ST_CODE_PENDING = 2
} e_status_code_t;
// Estructura para el contexto del servidor sin servidor
typedef struct
{
    prov_eng_cli_context_t m_t_prov_eng_context;
    char m_c_endpoint[256];
    char m_c_url_proto[10];

} serverless_runtime_context_t;

/******************* GLOBAL VARIABLES *********************/

/******************* PUBLIC METHODS ***********************/

// Init prov engine client with m_t_prov_eng_context.m_cognit_prov_engine_config
e_status_code_t init_serverless_runtime_context(cognit_config_t* config);

// Create m_t_prov_eng_context.m_serverless_runtime_context, then do prov_eng_create_runtime, look in prov engine response (JSON) the endpoint
e_status_code_t create_serverless_runtime(basic_serverless_runtime_conf_t t_basic_serverless_runtime_conf);

// Make prov_engline_clie retreive with serverless runtime id
const char* check_serverless_runtime_status();

// Función para realizar una llamada sincrónica al servidor sin servidor
exec_response_t call_sync(exec_faas_params_t* t_faas_params);

// Función para realizar una llamada asíncrona al servidor sin servidor
async_exec_response_t call_async(exec_faas_params_t* t_faas_params);

// Función para esperar la finalización de una tarea
const char* wait_for_task(const char* c_async_task_id, uint32_t ui32_timeout_ms);

// Función para eliminar el servidor sin servidor
void delete_serverless_runtime(const char* c_endpoint);

/******************* PRIVATE METHODS ***********************/
serverless_runtime_context_t m_t_serverless_runtime_context;

#endif // SERVERLESS_RUNTIME_CONTEXT_H
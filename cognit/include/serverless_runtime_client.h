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
#ifndef SERVERLESS_RUNTIME_CLIENT_H
#define SERVERLESS_RUNTIME_CLIENT_H
/********************** INCLUDES **************************/
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
/***************** DEFINES AND MACROS *********************/
#define FAAS_VERSION "v1"
#define FAAS_EXECUTE_SYNC_ENDPOINT "faas/execute-sync"
#define FAAS_EXECUTE_ASYNC_ENDPOINT "faas/execute-async"
#define FAAS_WAIT_ENDPOINT "faas/%s/status"

#define MODE_IN  "IN"
#define MODE_OUT "OUT"

/**************** TYPEDEFS AND STRUCTS ********************/
typedef enum
{
    OK      = 0,
    WORKING = 1,
    NOT_OK  = -1
} e_status_exec_t;

typedef struct
{
    char* state;
    char* result; // NULL if not present
} FaasUuidStatus;

typedef enum
{
    SUCCESS = 0,
    ERROR   = -1
} exec_return_code_t;

typedef struct
{
    exec_return_code_t ret_code;
    char* res_payload; // NULL if not present
    size_t res_payload_len;
    long http_err_code;
} exec_response_t;

typedef struct
{
    char* faas_task_uuid;
} AsyncExecId;

typedef struct
{
    char* status;         // "WORKING", "READY", "FAILED"
    exec_response_t* res; // NULL if not present
    AsyncExecId exec_id;
} async_exec_response_t;

/******************* GLOBAL VARIABLES *********************/

/******************* PUBLIC METHODS ***********************/

// Función para inicializar el contexto del servidor sin servidor
void init_serverless_runtime_cli(const char* c_endpoint);

exec_response_t faas_exec_sync(uint8_t* ui8_payload, size_t payload_len);

async_exec_response_t faas_exec_async(uint8_t* ui8_payload, size_t payload_len);

// Funcion wait que le pase un AsyncExecResponse y un intervalo de tiempo
async_exec_response_t waitForTask(const char* c_async_task_id, uint32_t ui32_timeout_ms);

/******************* PRIVATE METHODS ***********************/

#endif // SERVERLESS_RUNTIME_CLIENT_H
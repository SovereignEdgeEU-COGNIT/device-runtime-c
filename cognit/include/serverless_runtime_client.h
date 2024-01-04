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
#define FAAS_VERSION                "v1"
#define FAAS_EXECUTE_SYNC_ENDPOINT  "faas/execute-sync"
#define FAAS_EXECUTE_ASYNC_ENDPOINT "faas/execute-async"
#define FAAS_WAIT_ENDPOINT          "faas/%s/status"

#define MODE_IN        "IN"
#define MODE_OUT       "OUT"
#define MAX_URL_LENGTH 512

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
    char faas_task_uuid[256];
} AsyncExecId;

typedef struct
{
    char status[10];      // "WORKING", "READY", "FAILED"
    exec_response_t* res; // NULL if not present
    AsyncExecId exec_id;
} async_exec_response_t;

/******************* GLOBAL VARIABLES *********************/

/******************* PUBLIC METHODS ***********************/

/*******************************************************/ /**
 * @brief Init the serverless runtime client with the server endpoint
 * 
 * @param c_endpoint Server endpoint
***********************************************************/
void srcli_init(const char* c_endpoint);

/*******************************************************/ /**
 * @brief Check if the serverless runtime is created
 * 
 * @return true if created, otherwise false
***********************************************************/
bool srcli_there_is_srv_runtime_created();

/*******************************************************/ /**
 * @brief Makes a POST petition to the serverless runtime for sync task execution
 * 
 * @param ui8_payload JSON payload with the parameters
 * @param payload_len Length of the payload
 * @return exec_response_t Response of the serverless runtime
***********************************************************/
exec_response_t srcli_faas_exec_sync(uint8_t* ui8_payload, size_t payload_len);

/*******************************************************/ /**
 * @brief Makes a POST petition to the serverless runtime for async task execution
 * 
 * @param ui8_payload JSON payload with the parameters
 * @param payload_len Length of the payload
 * @return async_exec_response_t Response of the serverless runtime
***********************************************************/
async_exec_response_t srcli_faas_exec_async(uint8_t* ui8_payload, size_t payload_len);

/*******************************************************/ /**
 * @brief Makes a GET petition to the serverless runtime to know the status of an async task
 * 
 * @param c_async_task_id String with the async task ID
 * @param ui32_timeout_ms Timeout in milliseconds
 * @return async_exec_response_t Response of the serverless runtime
***********************************************************/
async_exec_response_t srcli_wait_for_task(const char* c_async_task_id, uint32_t ui32_timeout_ms);

/******************* PRIVATE METHODS ***********************/

#endif // SERVERLESS_RUNTIME_CLIENT_H
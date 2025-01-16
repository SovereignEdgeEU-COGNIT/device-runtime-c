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
#ifndef EDGE_CLUSTER_FRONTEND_CLI_H
#define EDGE_CLUSTER_FRONTEND_CLI_H
/********************** INCLUDES **************************/
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
/***************** DEFINES AND MACROS *********************/
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
    char status[10]; // "WORKING", "READY", "FAILED"
    exec_response_t res;
    AsyncExecId exec_id;
} async_exec_response_t;

typedef struct SEdgeClusterFrontendCli
{
    const char t_ecf_endpoint[MAX_URL_LENGTH];
    char* token;
    int has_connection;
    char c_a_exec_sync_url[MAX_URL_LENGTH];
    char c_a_exec_async_url[MAX_URL_LENGTH];
    char c_a_wait_task_url[MAX_URL_LENGTH];

} edge_cluster_frontend_cli_t;
/******************* GLOBAL VARIABLES *********************/

/******************* PUBLIC METHODS ***********************/

/*******************************************************/ /**
 * @brief Initializes the edge cluster frontend client with the server endpoint
 * 
 * This function initializes the edge cluster frontend client with the provided server endpoint.
 * 
 * @param pt_edge_cluster_frontend_cli Pointer to the edge cluster frontend client structure
 * @param token Session token
 * @param c_endpoint Server endpoint URL
***********************************************************/
void ecf_cli_init(edge_cluster_frontend_cli_t* pt_edge_cluster_frontend_cli, const char* c_endpoint);

/*******************************************************/ /**
 * @brief Executes a synchronous task on the edge_cluster
 * 
 * This function makes a POST request to the edge_cluster for synchronous task execution.
 * 
 * @param pt_edge_cluster_frontend_cli Pointer to the edge_cluster client structure
 * @param ui8_payload JSON payload with the task parameters
 * @param payload_len Length of the payload
 * @param pt_exec_response Pointer to the execution response structure
 * @return 0 on success, an error code otherwise
***********************************************************/
int ecf_cli_faas_exec_sync(edge_cluster_frontend_cli_t* pt_edge_cluster_frontend_cli, int func_id, uint8_t* ui8_payload, size_t payload_len, exec_response_t* pt_exec_response);

void ecf_set_has_connection(edge_cluster_frontend_cli_t* pt_edge_cluster_frontend_cli, bool value);

bool ecf_get_has_connection(edge_cluster_frontend_cli_t* pt_edge_cluster_frontend_cli);
/******************* PRIVATE METHODS ***********************/

#endif // EDGE_CLUSTER_CLI_H
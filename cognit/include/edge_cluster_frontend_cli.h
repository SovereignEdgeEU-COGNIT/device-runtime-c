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
#include "faas_parser.h"
/***************** DEFINES AND MACROS *********************/
#define MAX_URL_LENGTH        512
#define REQ_TIMEOUT           60
#define FUNC_UPLOAD_ENDPOINT  "v1/daas/c/upload_fc"
#define FAAS_REQUEST_ENDPOINT "v1/faas/c/faas_request"

/**************** TYPEDEFS AND STRUCTS ********************/

typedef struct SEdgeClusterFrontendCli
{
    const char t_ecf_endpoint[MAX_URL_LENGTH];
    int has_connection;
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
int ecf_cli_faas_exec_sync(edge_cluster_frontend_cli_t* pt_edge_cluster_frontend_cli, char* biscuit_token, faas_t* pt_faas, void** pt_exec_response);

int ecf_cli_upload_function_to_daas(edge_cluster_frontend_cli_t* pt_edge_cluster_frontend_cli, char* biscuit_token, faas_t* pt_faas);

void ecf_set_has_connection(edge_cluster_frontend_cli_t* pt_edge_cluster_frontend_cli, bool value);

bool ecf_get_has_connection(edge_cluster_frontend_cli_t* pt_edge_cluster_frontend_cli);
/******************* PRIVATE METHODS ***********************/

#endif // EDGE_CLUSTER_CLI_H
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
#ifndef COGNIT_FRONTEND_CLI_H
#define COGNIT_FRONTEND_CLI_H
/********************** INCLUDES **************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <cJSON.h>
#include <cognit_config.h>

/***************** DEFINES AND MACROS *********************/
#define MAX_ENERGY_SCHEDULING_POLICIES 1
#define NAME_MAX_LEN                   50

#define CF_AUTH_ENDPOINT        "v1/authenticate"
#define CF_REQ_ENDPOINT         "v1/app_requirements"
#define CF_ECF_ADDRESS_ENDPOINT "ec_fe"

#define MAX_URL_LENGTH 512
#define REQ_TIMEOUT    10

#define MAX_TOKEN_LENGTH 512
#define MAX_HOSTS        15
#define MAX_DATASTORES   15
#define MAX_VNETS        15

/**************** TYPEDEFS AND STRUCTS ********************/

typedef enum ECFCState
{
    E_CFC_STATE_ERROR  = -1,
    E_CFC_STATE_SUCCES = 0
} e_cfc_state_t;

typedef struct
{
    char flavour[NAME_MAX_LEN];
    int max_latency;
    float max_function_execution_time;
    int min_renewable;
    char geolocation[NAME_MAX_LEN];
} scheduling_t;

typedef struct SEdgeClusterFrontendResponse
{
    int id;
    char name[NAME_MAX_LEN];
    int hosts[MAX_HOSTS];
    int datastores[MAX_DATASTORES];
    int vnets[MAX_VNETS];
    char template[MAX_URL_LENGTH];
} ecf_response_t;

typedef struct SCognitFrontend
{
    cognit_config_t* m_t_config;
    ecf_response_t ecf_resp;
    bool has_connection;
} cognit_frontend_cli_t;

/******************* GLOBAL VARIABLES *********************/

/******************* PUBLIC METHODS ***********************/

/*******************************************************/ /**
 * @brief Initializes the cognit_frontend_cli_t structure with the given configuration.
 * 
 * This function loads the cognit configuration and validates it.
 * 
 * @param t_cognit_frontend_cli Pointer to the cognit_frontend_cli_t structure to initialize.
 * @param pt_cognit_config Pointer to the cognit_config_t structure containing the configuration.
 * @return int 0 if success, -1 otherwise.
***********************************************************/
e_cfc_state_t cognit_frontend_cli_init(cognit_frontend_cli_t* t_cognit_frontend_cli, cognit_config_t* pt_cognit_config);

/*******************************************************/ /**
 * @brief Authenticates to the cognit frontend and gets the token.
 * 
 * @param t_cognit_frontend_cli Pointer to the cognit_frontend_cli_t structure.
 * @return char pointer to the token.
***********************************************************/
e_cfc_state_t cognit_frontend_cli_authenticate(cognit_frontend_cli_t* t_cognit_frontend_cli, char* token);

/*******************************************************/ /**
 * @brief Updates the app requirement in the cognit frontend.
 * 
 * @param t_cognit_frontend_cli Pointer to the cognit_frontend_cli_t structure.
 * @param requirements_json app requirements.
 * @return int 0 if success, -1 otherwise.
***********************************************************/
int cognit_frontend_cli_update_requirements(cognit_frontend_cli_t* pt_cognit_frontend_cli, char* biscuit_token, scheduling_t t_app_reqs, int* pt_app_req_id);

/*******************************************************/ /**
 * @brief Gets the status of an edge cluster.
 * 
 * This function gets the address of an edge cluster from the cognit frontend.
 * 
 * @param t_cognit_frontend_cli Pointer to the cognit_frontend_cli_t structure.
 * @param ecf_address address of the edge cluster frontend.
 * @return int 0 if success, -1 otherwise.
***********************************************************/
e_cfc_state_t cognit_frontend_cli_get_ecf_address(cognit_frontend_cli_t* pt_cognit_frontend_cli, char* biscuit_token, int app_req_id);

void set_has_connection(cognit_frontend_cli_t* pt_cognit_frontend_cli, bool value);

bool cfc_get_has_connection(cognit_frontend_cli_t* pt_cognit_frontend_cli);

/******************* PRIVATE METHODS ***********************/
#endif // COGNIT_FRONTEND_CLI_H
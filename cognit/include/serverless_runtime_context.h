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
#include <cognit_config.h>
/***************** DEFINES AND MACROS *********************/
#define MODE_IN      "IN"
#define MODE_OUT     "OUT"
#define INTERVAL_1MS 1

#define SR_NAME_MAX_LEN              50
#define SR_FLAVOUR_MAX_LEN           50
#define SR_ENDPOINT_MAX_LEN          256
#define SR_STATE_MAX_LEN             256
#define SR_VM_ID_MAX_LEN             256
#define SR_POLICY_MAX_LEN            256
#define SR_REQ_MAX_LEN               256
#define SR_GEOGRAPH_LOCATION_MAX_LEN 256

#define MAX_ENERGY_SCHEDULING_POLICIES 1
#define POLICY_NAME_MAX_LEN            50
/**************** TYPEDEFS AND STRUCTS ********************/

typedef struct
{
    uint32_t ui32_energy_percentage;
} energy_scheduling_policy_t;

// Representación de la clase ServerlessRuntimeConfig
typedef struct
{
    energy_scheduling_policy_t m_t_energy_scheduling_policies[MAX_ENERGY_SCHEDULING_POLICIES];
    char name[SR_NAME_MAX_LEN];
    char faas_flavour[SR_FLAVOUR_MAX_LEN];
    char daas_flavour[SR_FLAVOUR_MAX_LEN];
} serverless_runtime_conf_t;

typedef struct
{
    uint8_t ui8_cpu;
    uint32_t ui32_memory;
    uint32_t ui32_disk_size;
    char c_flavour[SR_FLAVOUR_MAX_LEN];
    char c_endpoint[SR_ENDPOINT_MAX_LEN];
    char c_state[SR_STATE_MAX_LEN];
    char c_vm_id[SR_VM_ID_MAX_LEN];
} faas_config_t;

typedef faas_config_t daas_config_t;

typedef struct
{
    char c_policy[SR_POLICY_MAX_LEN];
    char c_requirements[SR_REQ_MAX_LEN];
} scheduling_config_t;

typedef struct
{
    uint32_t ui32_latency_to_pe;
    char c_geograph_location[SR_GEOGRAPH_LOCATION_MAX_LEN];
} device_info_t;

typedef struct
{
    char c_name[SR_NAME_MAX_LEN];
    uint32_t ui32_id;
    faas_config_t faas_config;
    daas_config_t daas_config;
    scheduling_config_t scheduling_config;
    device_info_t device_info;
} serverless_runtime_t;

typedef enum
{
    E_ST_CODE_SUCCESS = 0,
    E_ST_CODE_ERROR   = 1,
    E_ST_CODE_PENDING = 2
} e_status_code_t;

typedef struct
{
    cognit_config_t m_t_cognit_conf;
    serverless_runtime_t m_t_serverless_runtime;
    prov_engine_cli_t m_t_prov_engine_cli;
    char m_c_endpoint[256];

} serverless_runtime_context_t;

/******************* GLOBAL VARIABLES *********************/

/******************* PUBLIC METHODS ***********************/

/*******************************************************/ /**
 * @brief Initialize the serverless runtime context and shares pec_context with prov_engine_cli
 * 
 * @param pt_sr_ctx Serverless runtime context instance
 * @param pt_cfg Cognit library configuration
 * @return e_status_code_t Status returned by prov engine client
***********************************************************/
e_status_code_t serverless_runtime_ctx_init(serverless_runtime_context_t* pt_sr_ctx, const cognit_config_t* pt_cfg);

/*******************************************************/ /**
 * @brief Creates default serverless runtime configuration
 * 
 * @param pt_sr_ctx Serverless runtime context instance
 * @param t_basic_serverless_runtime_conf Basic serverless runtime configuration
 * @return e_status_code_t Status returned by prov engine client
***********************************************************/
e_status_code_t serverless_runtime_ctx_create(serverless_runtime_context_t* pt_sr_ctx, const serverless_runtime_conf_t* t_sr_conf);

/*******************************************************/ /**
 * @brief Gets current serverless runtime status
 * 
 * @return const char* Serverless runtime status
***********************************************************/
e_faas_state_t serverless_runtime_ctx_status(serverless_runtime_context_t* pt_sr_ctx);

/*******************************************************/ /**
 * @brief Parses faas_params, generates the payload and calls the sync serverless runtime
 * 
 * @param exec_faas_params Execution parameters
 * @return exec_response_t Execution response
***********************************************************/
exec_response_t srcontext_call_sync(exec_faas_params_t* exec_faas_params);

/*******************************************************/ /**
 * @brief Parses faas_params, generates the payload and calls the async serverless runtime
 * 
 * @param exec_faas_params Execution parameters
 * @return async_exec_response_t Execution response
***********************************************************/
async_exec_response_t srcontext_call_async(exec_faas_params_t* exec_faas_params);

/*******************************************************/ /**
 * @brief Ask periodically to the serverless runtime if the task has finished
 * 
 * @param c_async_task_id Task id to wait for
 * @param ui32_timeout_ms Timeout in milliseconds
 * @return async_exec_response_t Execution response
***********************************************************/
async_exec_response_t srcontext_wait_for_task(const char* c_async_task_id, uint32_t ui32_timeout_ms);

/*******************************************************/ /**
 * @brief Ask pec to delete the serverless runtime
 * 
 * @param c_endpoint Serverless runtime endpoint
***********************************************************/
void delete_serverless_runtime(const char* c_endpoint);

/******************* PRIVATE METHODS ***********************/

#endif // SERVERLESS_RUNTIME_CONTEXT_H
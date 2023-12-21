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
#ifndef PROV_ENGINE_CLI_H
#define PROV_ENGINE_CLI_H
/********************** INCLUDES **************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <cJSON.h>
/***************** DEFINES AND MACROS *********************/
#define SR_RESOURCE_ENDPOINT "serverless-runtimes"
#define REQ_TIMEOUT          60

#define PE_ERR_CODE_SUCCESS 0
#define PE_ERR_CODE_ERROR   1

// Definición de constantes de cadena para FaaSState
#define FaaSState_PENDING  "PENDING"
#define FaaSState_RUNNING  "RUNNING"
#define FaaSState_NO_STATE ""

#define PE_ENDPOINT_MAX_LEN 256
#define PE_USR_MAX_LEN      256
#define PE_PWD_MAX_LEN      256

#define SR_NAME_MAX_LEN              50
#define SR_FLAVOUR_MAX_LEN           50
#define SR_ENDPOINT_MAX_LEN          256
#define SR_STATE_MAX_LEN             256
#define SR_VM_ID_MAX_LEN             256
#define SR_POLICY_MAX_LEN            256
#define SR_REQ_MAX_LEN               256
#define SR_GEOGRAPH_LOCATION_MAX_LEN 256

#define MAX_SCHEDULING_POLICIES 10
#define POLICY_NAME_MAX_LEN     50

/**************** TYPEDEFS AND STRUCTS ********************/
typedef struct
{
    char prov_engine_endpoint[PE_ENDPOINT_MAX_LEN];
    uint32_t prov_engine_port;
    char prov_engine_pe_usr[PE_USR_MAX_LEN];
    char prov_engine_pe_pwd[PE_PWD_MAX_LEN];
    uint32_t ui32_serv_runtime_port;
    // Add other fields as needed
} cognit_config_t;

typedef struct
{
    char c_policy_name[POLICY_NAME_MAX_LEN];
    uint32_t ui32_energy_percentage;
} energy_scheduling_policy_t;

// Representación de la clase ServerlessRuntimeConfig
typedef struct
{
    energy_scheduling_policy_t scheduling_policies[MAX_SCHEDULING_POLICIES];
    char name[SR_NAME_MAX_LEN];
    char faas_flavour[SR_FLAVOUR_MAX_LEN];
    char daas_flavour[SR_FLAVOUR_MAX_LEN];
} basic_serverless_runtime_conf_t;

typedef struct
{
    uint8_t ui8_cpu;
    uint32_t ui32_memory;
    uint32_t ui32_disk_size;
    char c_flavour[SR_FLAVOUR_MAX_LEN];
    char c_endpoint[SR_ENDPOINT_MAX_LEN];
    char c_state[SR_STATE_MAX_LEN];
    char c_vm_id[SR_VM_ID_MAX_LEN];
} xaas_config_t;

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
    xaas_config_t faas_config;
    xaas_config_t daas_config;
    scheduling_config_t scheduling_config;
    device_info_t device_info;
} serverless_runtime_conf_t;

typedef struct
{
    cognit_config_t m_cognit_prov_engine_config;
    serverless_runtime_conf_t m_serverless_runtime_conf;
} prov_eng_cli_context_t;

/******************* GLOBAL VARIABLES *********************/

/******************* PUBLIC METHODS ***********************/

/*******************************************************/ /**
 * @brief Load cognit configuration to use in private m_cognit_prov_engine_config
 * and validate it
 * 
 * @param config Configuration to load
 * @return int8_t 0 if success, -1 otherwise
***********************************************************/
int8_t prov_eng_init(prov_eng_cli_context_t* t_pec_context);

/*******************************************************/ /**
 * @brief Create a runtime object in the provisioning engine and make HTTP request
 * 
 * @param serverless_runtime_context Serverless runtime context to create
 * @return cJSON JSON response from provisioning engine
***********************************************************/
serverless_runtime_conf_t prov_eng_create_runtime(serverless_runtime_conf_t* serverless_runtime_context);

/*******************************************************/ /**
 * @brief Retreives serverless runtime status
 * 
 * @param ui32_id Serverless runtime id
 * @return cJSON JSON response from provisioning engine
***********************************************************/
serverless_runtime_conf_t prov_eng_retreive_runtime(uint32_t ui32_id);

/*******************************************************/ /**
 * @brief Deletes serverless runtime
 * 
 * @param ui32_id Serverless runtime id
 * @return cJSON JSON response from provisioning engine
***********************************************************/
bool prov_eng_delete_runtime(uint32_t ui32_id);

/******************* PRIVATE METHODS ***********************/
prov_eng_cli_context_t* m_t_pec_context;
#endif // PROV_ENGINE_CLI_H
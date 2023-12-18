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
#define REQ_TIMEOUT 60

// Definición de constantes de cadena para FaaSState
#define FaaSState_PENDING "PENDING"
#define FaaSState_RUNNING "RUNNING"
#define FaaSState_NO_STATE ""

/**************** TYPEDEFS AND STRUCTS ********************/
typedef struct {
    char* prov_engine_endpoint;
    uint32_t prov_engine_port;
    char* _prov_engine_pe_usr;
    char* _prov_engine_pe_pwd;
    // Add other fields as needed
} congnit_config_t;

typedef struct {
    uint8_t ui8_cpu;
    uint32_t ui32_memory;
    uint32_t ui32_disk_size;
    char* c_flavour;
    char* c_endpoint;
    char* c_state;
    char* c_vm_id;
} xaas_config_t;

typedef struct {
    char* c_policy;
    char* c_requirements;
} scheduling_config_t;

typedef struct {
    uint32_t ui32_latency_to_pe;
    char c_geograph_location[256];
} device_info_t;

typedef struct {
    char* c_name;
    uint32_t ui32_id;
    xaas_config_t faas_config;
    xaas_config_t daas_config;
    scheduling_config_t scheduling_config;
    device_info_t device_info;
} serverless_runtime_conf_t;

typedef struct {
    congnit_config_t m_cognit_prov_engine_config;
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
int8_t prov_eng_init(congnit_config_t* config);

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
prov_eng_cli_context_t m_t_prov_eng_cli_get_context;
#endif // PROV_ENGINE_CLI_H
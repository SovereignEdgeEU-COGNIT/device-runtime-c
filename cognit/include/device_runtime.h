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
#ifndef DEVICE_RUNTIME_H
#define DEVICE_RUNTIME_H
/********************** INCLUDES **************************/
#include <stdio.h>
#include <stdbool.h>
#include <cognit_frontend_cli.h>
#include <cognit_config.h>
#include <device_runtime_state_machine.h>
/***************** DEFINES AND MACROS *********************/
#define FAAS_MAX_SEND_PAYLOD_SIZE 16384 // 16KB

/**************** TYPEDEFS AND STRUCTS ********************/

typedef struct SDeviceRuntime
{
    cognit_config_t m_t_config;
    device_runtime_sm_t m_t_device_runtime_sm;
} device_runtime_t;

/******************* GLOBAL VARIABLES *********************/

/******************* PUBLIC METHODS ***********************/

/*******************************************************/ /**
 * @brief Initialize the device runtime, get token from cognit frontend and send app requirements
 * 
 * @param pt_dr Device runtime instance
 * @param config_path Cognit library configuration path
  * @param t_reqs Requirements of the application
 * @return e_status_code_t Status returned by the cognit frontend client
***********************************************************/
e_status_code_t device_runtime_init(device_runtime_t* pt_dr, cognit_config_t t_config, scheduling_t t_reqs, faas_t* pt_faas);

/*******************************************************/ /**
 * @brief Parses faas_params, generates the payload and calls the edge cluster
 * 
 * @param pt_dr Device runtime instance
 * @param exec_faas_params Execution parameters
 * @param pt_exec_response Execution response
 * @return e_status_code_t Execution status
***********************************************************/
e_status_code_t device_runtime_call(device_runtime_t* pt_dr, faas_t* pt_faas, scheduling_t t_new_reqs, void** pt_exec_response);
/******************* PRIVATE METHODS ***********************/

#endif // DEVICE_RUNTIME_H
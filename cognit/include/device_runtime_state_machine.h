#ifndef DEVICE_RUNTIME_SM_H
#define DEVICE_RUNTIME_SM_H

#include "cognit_frontend_cli.h"
#include "edge_cluster_frontend_cli.h"

// Enumeraciones para los estados y eventos
typedef enum {
    INIT,
    SEND_INIT_REQUEST,
    GET_ECF_ADDRESS,
    READY
} State_t;

typedef enum {
    ADDRESS_OBTAINED,
    TOKEN_NOT_VALID_ADDRESS,
    RETRY_GET_ADDRESS,
    LIMIT_GET_ADDRESS,
    ADDRESS_UPDATE_REQUIREMENTS,
    SUCCESS_AUTH,
    REPEAT_AUTH,
    RESULT_GIVEN,
    TOKEN_NOT_VALID_READY,
    TOKEN_NOT_VALID_READY_2,
    READY_UPDATE_REQUIREMENTS,
    REQUIREMENTS_UP,
    TOKEN_NOT_VALID_REQUIREMENTS,
    RETRY_REQUIREMENTS_UPLOAD,
    LIMIT_REQUIREMENTS_UPLOAD,
    SEND_INIT_UPDATE_REQUIREMENTS
} Event_t;

// State machine transitions
typedef struct SDeviceRuntimeStateMachine{
    State_t current_state;
    cognit_config_t m_t_config;
    scheduling_t m_t_requirements;
    char biscuit_token[MAX_TOKEN_LENGTH];  /**< The biscuit token. */
    cognit_frontend_cli_t cfc;
    edge_cluster_frontend_cli_t ecf;
    int up_req_counter;
    int get_address_counter;
} device_runtime_sm_t;

// Estructura de transición de estado
typedef struct STransition{
    State_t previous_state;
    Event_t event;
    State_t next_state;
    int (*check_conditions)(device_runtime_sm_t* pt_dr_sm);
} sm_transition_t;

// Declaraciones de funciones
int execute_transition(device_runtime_sm_t* pt_dr_sm, Event_t event);

int device_runtime_sm_init(device_runtime_sm_t* pt_dr_state_machine, cognit_config_t t_config, scheduling_t t_reqs);

exec_response_t offload_function(device_runtime_sm_t* pt_dr_state_machine);


#endif // DEVICE_RUNTIME_STATE_MACHINE_H
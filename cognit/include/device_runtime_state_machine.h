#ifndef DEVICE_RUNTIME_SM_H
#define DEVICE_RUNTIME_SM_H

#include "cognit_frontend_cli.h"
#include "edge_cluster_frontend_cli.h"

#define MAX_REQ_UPLOAD_ATTEMPTS     3
#define MAX_GET_ADDRESS_ATTEMPTS    3

typedef enum
{
    E_ST_CODE_SUCCESS = 0,
    E_ST_CODE_ERROR   = -1
} e_status_code_t;


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
    cognit_frontend_cli_t cfc;
    edge_cluster_frontend_cli_t ecf;
    char biscuit_token[MAX_TOKEN_LENGTH];  /**< The biscuit token. */
    int app_req_id;
    scheduling_t m_t_requirements;
    int up_req_counter;
    int get_address_counter;
    bool requirements_changed;
    bool requirements_uploaded;
    int has_requirements_upload_limit_reached;
    int has_address_request_limit_reached;
} device_runtime_sm_t;

// Estructura de transición de estado
typedef struct STransition{
    State_t previous_state;
    Event_t event;
    State_t next_state;
    int (*check_conditions)(device_runtime_sm_t* pt_dr_sm);
} sm_transition_t;

// Declaraciones de funciones
int dr_state_machine_execute_transition(device_runtime_sm_t* pt_dr_sm, Event_t event);

int dr_state_machine_init(device_runtime_sm_t* pt_dr_state_machine, cognit_config_t t_config);

exec_response_t dr_sm_offload_function(device_runtime_sm_t* pt_dr_sm);

e_status_code_t dr_sm_update_requirements(device_runtime_sm_t* pt_dr_sm, scheduling_t t_reqs);

#endif // DEVICE_RUNTIME_STATE_MACHINE_H
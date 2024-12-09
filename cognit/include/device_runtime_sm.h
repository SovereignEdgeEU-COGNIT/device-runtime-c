#ifndef DEVICE_RUNTIME_SM_H
#define DEVICE_RUNTIME_SM_H

#include "cognit_frontend_cli.h"
#include "edge_cluster_cli.h"

// Enumeraciones para los estados y eventos
typedef enum {
    GET_ECF_ADDRESS,
    INIT,
    READY,
    SEND_INIT_REQUEST
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

// Estructura de transición de estado
typedef struct {
    State_t previous_state;
    Event_t event;
    State_t next_state;
    int (*check_conditions)(device_runtime_state_machine_t t_dr_sm, Event_t event);
} sm_transition_t;

// Declaraciones de funciones
void execute_transition(device_runtime_state_machine_t t_dr_sm, Event event);

// State machine transitions
typedef struct SDeviceRuntimeStateMachine{
    State_t current_state;
    cognit_frontend_cli_t *cfc;
    edge_cluster_cli_t *ecf;
} device_runtime_state_machine_t;

void device_runtime_sm_init(device_runtime_state_machine_t t_dr_state_machine);

exec_response_t offload_function(device_runtime_state_machine_t t_dr_state_machine, whatever );


#endif // DEVICE_RUNTIME_STATE_MACHINE_H
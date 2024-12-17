#include <device_runtime_state_machine.h>
#include "logger.h"

exec_response_t offload_function(device_runtime_sm_t* pt_dr_state_machine);

static int address_obtained_condition(device_runtime_sm_t* pt_dr_sm) {
    printf("Check ADDRESS_OBTAINED conditions\n");
    return 0;
}

static int token_not_valid_address_condition(device_runtime_sm_t* pt_dr_sm) {
    printf("Check TOKEN_NOT_VALID_ADDRESS conditions\n");
    return 0;
}

static int retry_get_address_condition(device_runtime_sm_t* pt_dr_sm) {
    printf("Check RETRY_GET_ADDRESS conditions\n");
    return 0;
}

static int limit_get_address_condition(device_runtime_sm_t* pt_dr_sm) {
    printf("Check LIMIT_GET_ADDRESS conditions\n");
    return 0;
}

static int address_update_requirements_condition(device_runtime_sm_t* pt_dr_sm) {
    printf("Check ADDRESS_UPDATE_REQUIREMENTS conditions\n");
    return 0;
}

static int success_auth_condition(device_runtime_sm_t* pt_dr_sm) {
    printf("Check SUCCESS_AUTH conditions\n");
    return 0;
}

static int repeat_auth_condition(device_runtime_sm_t* pt_dr_sm) {
    printf("Check REPEAT_AUTH conditions\n");
    return 0;
}

static int result_given_condition(device_runtime_sm_t* pt_dr_sm) {
    printf("Check RESULT_GIVEN conditions\n");
    return 0;
}

static int token_not_valid_ready_condition(device_runtime_sm_t* pt_dr_sm) {
    printf("Check TOKEN_NOT_VALID_READY conditions\n");
    return 0;
}

static int token_not_valid_ready_2_condition(device_runtime_sm_t* pt_dr_sm) {
    printf("Check TOKEN_NOT_VALID_READY_2 conditions\n");
    return 0;
}

static int ready_update_requirements_condition(device_runtime_sm_t* pt_dr_sm) {
    printf("Check READY_UPDATE_REQUIREMENTS conditions\n");
    return 0;
}

static int requirements_up_condition(device_runtime_sm_t* pt_dr_sm) {
    printf("Check REQUIREMENTS_UP conditions\n");
    return 0;
}

static int token_not_valid_requirements_condition(device_runtime_sm_t* pt_dr_sm) {
    printf("Check TOKEN_NOT_VALID_REQUIREMENTS conditions\n");
    return 0;
}

static int retry_requirements_upload_condition(device_runtime_sm_t* pt_dr_sm) {
    printf("Check RETRY_REQUIREMENTS_UPLOAD conditions\n");
    return 0;
}

static int limit_requirements_upload_condition(device_runtime_sm_t* pt_dr_sm) {
    printf("Check LIMIT_REQUIREMENTS_UPLOAD conditions\n");
    return 0;
}

static int send_init_update_requirements_condition(device_runtime_sm_t* pt_dr_sm) {
    printf("Check SEND_INIT_UPDATE_REQUIREMENTS conditions\n");
    return 0;
}

// Transition table for state machine
sm_transition_t transitions[] = {
    {GET_ECF_ADDRESS, ADDRESS_OBTAINED, READY, address_obtained_condition},
    {GET_ECF_ADDRESS, TOKEN_NOT_VALID_ADDRESS, INIT, token_not_valid_address_condition},
    {GET_ECF_ADDRESS, RETRY_GET_ADDRESS, GET_ECF_ADDRESS, retry_get_address_condition},
    {GET_ECF_ADDRESS, LIMIT_GET_ADDRESS, INIT, limit_get_address_condition},
    {GET_ECF_ADDRESS, ADDRESS_UPDATE_REQUIREMENTS, SEND_INIT_REQUEST, address_update_requirements_condition},
    {INIT, SUCCESS_AUTH, SEND_INIT_REQUEST, success_auth_condition},
    {INIT, REPEAT_AUTH, INIT, repeat_auth_condition},
    {READY, RESULT_GIVEN, READY, result_given_condition},
    {READY, TOKEN_NOT_VALID_READY, INIT, token_not_valid_ready_condition},
    {READY, TOKEN_NOT_VALID_READY_2, INIT, token_not_valid_ready_2_condition},
    {READY, READY_UPDATE_REQUIREMENTS, SEND_INIT_REQUEST, ready_update_requirements_condition},
    {SEND_INIT_REQUEST, REQUIREMENTS_UP, GET_ECF_ADDRESS, requirements_up_condition},
    {SEND_INIT_REQUEST, TOKEN_NOT_VALID_REQUIREMENTS, INIT, token_not_valid_requirements_condition},
    {SEND_INIT_REQUEST, RETRY_REQUIREMENTS_UPLOAD, SEND_INIT_REQUEST, retry_requirements_upload_condition},
    {SEND_INIT_REQUEST, LIMIT_REQUIREMENTS_UPLOAD, INIT, limit_requirements_upload_condition},
    {SEND_INIT_REQUEST, SEND_INIT_UPDATE_REQUIREMENTS, SEND_INIT_REQUEST, send_init_update_requirements_condition}
};

// State functions
static void get_ecf_address_action(device_runtime_sm_t* pt_dr_sm) {
    printf("Action: Getting ECF address...\n");
}

static void init_action(device_runtime_sm_t* pt_dr_sm) {

    pt_dr_sm->up_req_counter = 0;
    pt_dr_sm->get_address_counter = 0;
    
    cognit_frontend_cli_init(&pt_dr_sm->cfc, &pt_dr_sm->m_t_config);
    cognit_frontend_cli_authenticate(&pt_dr_sm->cfc, pt_dr_sm->biscuit_token);

    COGNIT_LOG_DEBUG("Token: %s", pt_dr_sm->biscuit_token);
}

static void ready_action(device_runtime_sm_t* pt_dr_sm) {
    printf("Action: Ready for processing...\n");
}

static void send_init_request_action(device_runtime_sm_t* pt_dr_sm) {
    printf("Action: Sending initial request...\n");
}

static void execute_action(device_runtime_sm_t* pt_dr_sm)
{
    // Display current state
    switch (pt_dr_sm->current_state) {
        case INIT:
            COGNIT_LOG_DEBUG("Current state: INIT");
            init_action(pt_dr_sm);
            break;
        case SEND_INIT_REQUEST:
            COGNIT_LOG_DEBUG("Current state: SEND_INIT_REQUEST\n");
            send_init_request_action(pt_dr_sm);
            break;
        case GET_ECF_ADDRESS:
            COGNIT_LOG_DEBUG("Current state: GET_ECF_ADDRESS");
            get_ecf_address_action(pt_dr_sm);
            break;
        case READY:
            printf("Current state: READY");
            ready_action(pt_dr_sm);
            break;
        default:
            COGNIT_LOG_ERROR("Invalid state");
    }
}

// Function to get the next state based on the current state and event
int dr_state_machine_execute_transition(device_runtime_sm_t* pt_dr_sm, Event_t event) {
    for (int i = 0; i < sizeof(transitions) / sizeof(sm_transition_t); i++) {
        if (transitions[i].previous_state == pt_dr_sm->current_state && transitions[i].event == event) {
            int ret = transitions[i].check_conditions(pt_dr_sm);
            if (ret)
            {
                execute_action(pt_dr_sm);
                return 0;
            }
            else
            {
                COGNIT_LOG_ERROR("Conditions for transition not met\n");
                return -1;
            }
        }
    }
    COGNIT_LOG_ERROR("Invalid transition for current state");
    return -1;
}

int dr_state_machine_init(device_runtime_sm_t* pt_dr_sm, cognit_config_t t_config, scheduling_t t_reqs)
{
    if(pt_dr_sm == NULL)
    {
        COGNIT_LOG_ERROR("Device runtime state machine not initialized");
        return -1;
    }

    pt_dr_sm->current_state = INIT;
    pt_dr_sm->m_t_config = t_config;
    pt_dr_sm->m_t_requirements = t_reqs;
    
    COGNIT_LOG_DEBUG("Starting state machine")
    execute_action(pt_dr_sm);

    /////Esto no va aqui
    cognit_frontend_cli_update_requirements(&pt_dr_sm->cfc, pt_dr_sm->biscuit_token, pt_dr_sm->m_t_requirements, &pt_dr_sm->cfc.app_req_id);
    cognit_frontend_cli_get_ecf_address(&pt_dr_sm->cfc, pt_dr_sm->biscuit_token, pt_dr_sm->cfc.app_req_id);
    strcpy(pt_dr_sm->ecf.t_ecf_endpoint, pt_dr_sm->cfc.ecf_resp.template);

    return 0;
}


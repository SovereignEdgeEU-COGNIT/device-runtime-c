#include <device_runtime_sm.h>

int address_obtained_condition(device_runtime_sm_t t_dr_sm) {
    printf("Check ADDRESS_OBTAINED conditions\n");
    return 0;
}

int token_not_valid_address_condition(device_runtime_sm_t t_dr_sm) {
    printf("Check TOKEN_NOT_VALID_ADDRESS conditions\n");
    return 0;
}

int retry_get_address_condition(device_runtime_sm_t t_dr_sm) {
    printf("Check RETRY_GET_ADDRESS conditions\n");
    return 0;
}

int limit_get_address_condition(device_runtime_sm_t t_dr_sm) {
    printf("Check LIMIT_GET_ADDRESS conditions\n");
    return 0;
}

int address_update_requirements_condition(device_runtime_sm_t t_dr_sm) {
    printf("Check ADDRESS_UPDATE_REQUIREMENTS conditions\n");
    return 0;
}

int success_auth_condition(device_runtime_sm_t t_dr_sm) {
    printf("Check SUCCESS_AUTH conditions\n");
    return 0;
}

int repeat_auth_condition(device_runtime_sm_t t_dr_sm) {
    printf("Check REPEAT_AUTH conditions\n");
    return 0;
}

int result_given_condition(device_runtime_sm_t t_dr_sm) {
    printf("Check RESULT_GIVEN conditions\n");
    return 0;
}

int token_not_valid_ready_condition(device_runtime_sm_t t_dr_sm) {
    printf("Check TOKEN_NOT_VALID_READY conditions\n");
    return 0;
}

int token_not_valid_ready_2_condition(device_runtime_sm_t t_dr_sm) {
    printf("Check TOKEN_NOT_VALID_READY_2 conditions\n");
    return 0;
}

int ready_update_requirements_condition(device_runtime_sm_t t_dr_sm) {
    printf("Check READY_UPDATE_REQUIREMENTS conditions\n");
    return 0;
}

int requirements_up_condition(device_runtime_sm_t t_dr_sm) {
    printf("Check REQUIREMENTS_UP conditions\n");
    return 0;
}

int token_not_valid_requirements_condition(device_runtime_sm_t t_dr_sm) {
    printf("Check TOKEN_NOT_VALID_REQUIREMENTS conditions\n");
    return 0;
}

int retry_requirements_upload_condition(device_runtime_sm_t t_dr_sm) {
    printf("Check RETRY_REQUIREMENTS_UPLOAD conditions\n");
    return 0;
}

int limit_requirements_upload_condition(device_runtime_sm_t t_dr_sm) {
    printf("Check LIMIT_REQUIREMENTS_UPLOAD conditions\n");
    return 0;
}

int send_init_update_requirements_condition(device_runtime_sm_t t_dr_sm) {
    printf("Check SEND_INIT_UPDATE_REQUIREMENTS conditions\n");
    return 0;
}

// Transition table for state machine
sm_transition_t transitions[] = {
    {GET_ECF_ADDRESS, ADDRESS_OBTAINED, READY, address_obtained_condition},
    {GET_ECF_ADDRESS, TOKEN_NOT_VALID_ADDRESS, INIT, token_not_valid_address_condition},
    {GET_ECF_ADDRESS, RETRY_GET_ADDRESS, GET_ECF_ADDRESS, retry_get_address_condition},
    {GET_ECF_ADDRESS, LIMIT_GET_ADDRESS, INITlimit_get_address_congition},
    {GET_ECF_ADDRESS, ADDRESS_UPDATE_REQUIREMENTS, SEND_INIT_REQUEST, address_update_requirements_condition},
    {INIT, SUCCESS_AUTH, SEND_INIT_REQUEST, success_auth_condition},
    {INIT, REPEAT_AUTH, INITrepeat_aut_condition},
    {READY, RESULT_GIVEN, READYresult_given_condition},
    {READY, TOKEN_NOT_VALID_READY, INIT, token_not_valid_ready_condition},
    {READY, TOKEN_NOT_VALID_READY_2, INIT, token_not_valid_ready_2_condition},
    {READY, READY_UPDATE_REQUIREMENTS, SEND_INIT_REQUESTready_update_requirements_condition},
    {SEND_INIT_REQUEST, REQUIREMENTS_UP, GET_ECF_ADDRESS, requirements_up_condition},
    {SEND_INIT_REQUEST, TOKEN_NOT_VALID_REQUIREMENTS, INITtoken_not_valid_requirements_condition},
    {SEND_INIT_REQUEST, RETRY_REQUIREMENTS_UPLOAD, SEND_INIT_REQUEST, retry_requirements_upload_condition},
    {SEND_INIT_REQUEST, LIMIT_REQUIREMENTS_UPLOAD, INIT, limit_requirements_upload_condition},
    {SEND_INIT_REQUEST, SEND_INIT_UPDATE_REQUIREMENTS, SEND_INIT_REQUEST, send_init_update_requirements_condition}
};

// State functions
void get_ecf_address_action() {
    printf("Action: Getting ECF address...\n");
}

void init_action() {
    printf("Action: Initializing...\n");
}

void ready_action() {
    printf("Action: Ready for processing...\n");
}

void send_init_request_action() {
    printf("Action: Sending initial request...\n");
}

void execute_action(State_t current_state)
{
    // Display current state
    switch (current_state) {
        case INIT:
            printf("Current state: INIT\n");
            init_action();
            break;
        case SEND_INIT_REQUEST:
            printf("Current state: SEND_INIT_REQUEST\n");
            send_init_request_action();
            break;
        case GET_ECF_ADDRESS:
            printf("Current state: GET_ECF_ADDRESS\n");
            get_ecf_address_action();
            break;
        case READY:
            printf("Current state: READY\n");
            ready_action();
            break;
        default:
            printf("Invalid state\n");
            exit(1);
    }
}

// Function to get the next state based on the current state and event
int execute_transition(device_runtime_state_machine_t t_dr_sm, Event event) {
    for (int i = 0; i < sizeof(transitions) / sizeof(sm_transition_t); i++) {
        if (transitions[i].previous_state == t_dr_sm.current_state && transitions[i].event == event) {
            int ret = transitions[i].check_conditions();
            if (ret)
            {
                execute_action(t_dr_sm.current_state);
                return 0;
            }
            else
            {
                printf("Conditions for transition not met\n");
            }
        }
    }
    return -1;
}


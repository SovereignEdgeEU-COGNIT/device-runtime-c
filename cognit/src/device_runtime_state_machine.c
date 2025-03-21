#include <device_runtime_state_machine.h>
#include "logger.h"
#include <string.h>

bool is_cfc_connected(device_runtime_sm_t* pt_dr_sm)
{
    return cfc_get_has_connection(&pt_dr_sm->cfc);
}

bool is_ecf_connected(device_runtime_sm_t* pt_dr_sm)
{
    return ecf_get_has_connection(&pt_dr_sm->ecf);
}

bool have_requirements_changed(device_runtime_sm_t* pt_dr_sm)
{
    return pt_dr_sm->requirements_changed;
}

bool is_get_address_limit_reached(device_runtime_sm_t* pt_dr_sm)
{
    return pt_dr_sm->get_address_counter >= MAX_GET_ADDRESS_ATTEMPTS;
}

bool is_token_empty(device_runtime_sm_t* pt_dr_sm)
{
    return pt_dr_sm->biscuit_token[0] == '\0';
}

bool are_requirements_uploaded(device_runtime_sm_t* pt_dr_sm)
{
    return pt_dr_sm->requirements_uploaded;
}

bool is_requirement_upload_limit_reached(device_runtime_sm_t* pt_dr_sm)
{
    return pt_dr_sm->has_requirements_upload_limit_reached;
}

static void handle_init_state(device_runtime_sm_t* pt_dr_sm)
{
    if (!is_token_empty)
    {
        dr_state_machine_execute_transition(pt_dr_sm, SUCCESS_AUTH);
    }
    else
    {
        dr_state_machine_execute_transition(pt_dr_sm, REPEAT_AUTH);
    }
}

static void handle_send_init_request_state(device_runtime_sm_t* pt_dr_sm)
{
    if (cfc_get_has_connection(&pt_dr_sm->cfc))
    {
        if (have_requirements_changed(pt_dr_sm))
        {
            COGNIT_LOG_DEBUG("Requirements have changed, re-uploading...");
            dr_state_machine_execute_transition(pt_dr_sm, SEND_INIT_UPDATE_REQUIREMENTS);
        }
        else
        {
            if (is_requirement_upload_limit_reached)
            {
                COGNIT_LOG_DEBUG("Retrying to upload requirements...");
                dr_state_machine_execute_transition(pt_dr_sm, RETRY_REQUIREMENTS_UPLOAD);
            }
            else
            {
                COGNIT_LOG_DEBUG("Requirements uploaded succesfully, moving to get ECF address...");
                dr_state_machine_execute_transition(pt_dr_sm, REQUIREMENTS_UP);
            }
        }
    }
    else
    {
        COGNIT_LOG_DEBUG("Cognit Frontend Client disconnected, re-authenticating...");
        dr_state_machine_execute_transition(pt_dr_sm, TOKEN_NOT_VALID_REQUIREMENTS);
    }
}

static void handle_get_ecf_address_state(device_runtime_sm_t* pt_dr_sm)
{
    if (cfc_get_has_connection(&pt_dr_sm->cfc))
    {
        if (have_requirements_changed(pt_dr_sm))
        {
            COGNIT_LOG_DEBUG("Requirements have changed during address fetch, re-uploading...");
            dr_state_machine_execute_transition(pt_dr_sm, ADDRESS_UPDATE_REQUIREMENTS);
        }
        else
        {
            if (is_get_address_limit_reached(pt_dr_sm))
            {
                COGNIT_LOG_DEBUG("ECF address fetch limit reached, restarting client.");
                dr_state_machine_execute_transition(pt_dr_sm, LIMIT_GET_ADDRESS);
            }
            else if (ecf_get_has_connection(&pt_dr_sm->ecf))
            {
                COGNIT_LOG_DEBUG("ECF address obtained, transitioning to READY.");
                dr_state_machine_execute_transition(pt_dr_sm, ADDRESS_OBTAINED);
            }
            else
            {
                COGNIT_LOG_DEBUG("Retrying to fetch ECF address...");
                dr_state_machine_execute_transition(pt_dr_sm, RETRY_GET_ADDRESS);
            }
        }
    }
    else
    {
        COGNIT_LOG_DEBUG("Cognit Frontend Client disconnected, re-authenticating...");
        dr_state_machine_execute_transition(pt_dr_sm, TOKEN_NOT_VALID_ADDRESS);
    }
}

static void handle_ready_state(device_runtime_sm_t* pt_dr_sm)
{
    if (have_requirements_changed(pt_dr_sm))
    {
        COGNIT_LOG_DEBUG("Requirements changed, uploading updated requirements...");
        dr_state_machine_execute_transition(pt_dr_sm, READY_UPDATE_REQUIREMENTS);
    }
    else if (!cfc_get_has_connection(&pt_dr_sm->cfc))
    {
        COGNIT_LOG_DEBUG("Cognit Frontend Client disconnected, re-authenticating...");
        dr_state_machine_execute_transition(pt_dr_sm, TOKEN_NOT_VALID_READY);
    }
    else if (!ecf_get_has_connection(&pt_dr_sm->ecf))
    {
        COGNIT_LOG_DEBUG("Edge Cluster Frontend Client disconnected, re-authenticating...");
        dr_state_machine_execute_transition(pt_dr_sm, TOKEN_NOT_VALID_READY_2);
    }
}

static void handle_transitions(device_runtime_sm_t* pt_dr_sm)
{
    switch (pt_dr_sm->current_state)
    {
        case INIT:
            handle_init_state(pt_dr_sm);
            break;
        case SEND_INIT_REQUEST:
            handle_send_init_request_state(pt_dr_sm);
            break;
        case GET_ECF_ADDRESS:
            handle_get_ecf_address_state(pt_dr_sm);
            break;
        case READY:
            handle_ready_state(pt_dr_sm);
            break;
    }
}

static e_status_code_t exec_offload_func(device_runtime_sm_t* pt_dr_sm, faas_t* pt_faas, void** pt_exec_response)
{
    //if ya se subio, simplemente devolver ID, si no añadir a array o algo
    COGNIT_LOG_INFO("Sending function to DaaS");
    int fc_id = cfc_cli_upload_function_to_daas(&pt_dr_sm->cfc, pt_dr_sm->biscuit_token, pt_faas);

    if (fc_id != 0)
    {
        pt_faas->fc_id = fc_id;

        COGNIT_LOG_INFO("Executing FaaS request...");

        COGNIT_LOG_INFO("Executing FaaS request...");

        COGNIT_LOG_DEBUG("Waiting for result...");
        e_status_code_t ret = ecf_cli_faas_exec_sync(&pt_dr_sm->ecf, pt_dr_sm->biscuit_token, pt_dr_sm->app_req_id, pt_faas, pt_exec_response);

        //Borrar datos de pt_faas?????

        return ret;
    }

    return E_ST_CODE_ERROR;
}

void dr_sm_offload_function(device_runtime_sm_t* pt_dr_sm, faas_t* pt_faas, void** pt_exec_response)
{
    if (pt_dr_sm->current_state == READY)
    {
        exec_offload_func(pt_dr_sm, pt_faas, pt_exec_response);
    }
    else
    {
        COGNIT_LOG_DEBUG("State is not READY. Handling transitions...");
        handle_transitions(pt_dr_sm);
        COGNIT_LOG_DEBUG("Retrying function offload after state transitions...");
        exec_offload_func(pt_dr_sm, pt_faas, pt_exec_response);
    }
}

static int address_obtained_condition(device_runtime_sm_t* pt_dr_sm)
{
    if (is_ecf_connected(pt_dr_sm) && is_cfc_connected(pt_dr_sm) && !have_requirements_changed(pt_dr_sm))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int token_not_valid_address_condition(device_runtime_sm_t* pt_dr_sm)
{
    if (!is_cfc_connected(pt_dr_sm))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int retry_get_address_condition(device_runtime_sm_t* pt_dr_sm)
{
    if (!is_ecf_connected(pt_dr_sm) && is_cfc_connected(pt_dr_sm) && !is_get_address_limit_reached(pt_dr_sm))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int limit_get_address_condition(device_runtime_sm_t* pt_dr_sm)
{
    if (!is_ecf_connected(pt_dr_sm) && is_cfc_connected(pt_dr_sm) && is_get_address_limit_reached(pt_dr_sm))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int address_update_requirements_condition(device_runtime_sm_t* pt_dr_sm)
{
    if (have_requirements_changed(pt_dr_sm) && is_cfc_connected(pt_dr_sm))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int success_auth_condition(device_runtime_sm_t* pt_dr_sm)
{
    if (!is_token_empty(pt_dr_sm))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int repeat_auth_condition(device_runtime_sm_t* pt_dr_sm)
{
    if (is_token_empty(pt_dr_sm))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int result_given_condition(device_runtime_sm_t* pt_dr_sm)
{
    if (is_ecf_connected(pt_dr_sm) && is_cfc_connected(pt_dr_sm) && !have_requirements_changed(pt_dr_sm))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int token_not_valid_ready_condition(device_runtime_sm_t* pt_dr_sm)
{
    if (!is_cfc_connected(pt_dr_sm))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int token_not_valid_ready_2_condition(device_runtime_sm_t* pt_dr_sm)
{
    if (!is_ecf_connected(pt_dr_sm))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int ready_update_requirements_condition(device_runtime_sm_t* pt_dr_sm)
{
    if (is_ecf_connected(pt_dr_sm) && is_cfc_connected(pt_dr_sm) && have_requirements_changed(pt_dr_sm))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int requirements_up_condition(device_runtime_sm_t* pt_dr_sm)
{
    if (are_requirements_uploaded(pt_dr_sm) && is_cfc_connected(pt_dr_sm) && !have_requirements_changed(pt_dr_sm))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int token_not_valid_requirements_condition(device_runtime_sm_t* pt_dr_sm)
{
    if (!is_cfc_connected(pt_dr_sm))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int retry_requirements_upload_condition(device_runtime_sm_t* pt_dr_sm)
{
    if (have_requirements_changed(pt_dr_sm) && is_cfc_connected(pt_dr_sm) && !are_requirements_uploaded(pt_dr_sm) && !is_requirement_upload_limit_reached(pt_dr_sm))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int limit_requirements_upload_condition(device_runtime_sm_t* pt_dr_sm)
{
    if (is_requirement_upload_limit_reached(pt_dr_sm) && is_cfc_connected(pt_dr_sm) && !are_requirements_uploaded(pt_dr_sm))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int send_init_update_requirements_condition(device_runtime_sm_t* pt_dr_sm)
{
    if (have_requirements_changed(pt_dr_sm) && is_cfc_connected(pt_dr_sm))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// Transition table for state machine
sm_transition_t transitions[] = {
    { GET_ECF_ADDRESS, ADDRESS_OBTAINED, READY, address_obtained_condition },
    { GET_ECF_ADDRESS, TOKEN_NOT_VALID_ADDRESS, INIT, token_not_valid_address_condition },
    { GET_ECF_ADDRESS, RETRY_GET_ADDRESS, GET_ECF_ADDRESS, retry_get_address_condition },
    { GET_ECF_ADDRESS, LIMIT_GET_ADDRESS, INIT, limit_get_address_condition },
    { GET_ECF_ADDRESS, ADDRESS_UPDATE_REQUIREMENTS, SEND_INIT_REQUEST, address_update_requirements_condition },
    { INIT, SUCCESS_AUTH, SEND_INIT_REQUEST, success_auth_condition },
    { INIT, REPEAT_AUTH, INIT, repeat_auth_condition },
    { READY, RESULT_GIVEN, READY, result_given_condition },
    { READY, TOKEN_NOT_VALID_READY, INIT, token_not_valid_ready_condition },
    { READY, TOKEN_NOT_VALID_READY_2, INIT, token_not_valid_ready_2_condition },
    { READY, READY_UPDATE_REQUIREMENTS, SEND_INIT_REQUEST, ready_update_requirements_condition },
    { SEND_INIT_REQUEST, REQUIREMENTS_UP, GET_ECF_ADDRESS, requirements_up_condition },
    { SEND_INIT_REQUEST, TOKEN_NOT_VALID_REQUIREMENTS, INIT, token_not_valid_requirements_condition },
    { SEND_INIT_REQUEST, RETRY_REQUIREMENTS_UPLOAD, SEND_INIT_REQUEST, retry_requirements_upload_condition },
    { SEND_INIT_REQUEST, LIMIT_REQUIREMENTS_UPLOAD, INIT, limit_requirements_upload_condition },
    { SEND_INIT_REQUEST, SEND_INIT_UPDATE_REQUIREMENTS, SEND_INIT_REQUEST, send_init_update_requirements_condition }
};

// State functions
static void get_ecf_address_action(device_runtime_sm_t* pt_dr_sm)
{
    COGNIT_LOG_DEBUG("Entering GET ECF ADDRESS state");
    pt_dr_sm->up_req_counter = 0;
    pt_dr_sm->get_address_counter++;
    int ret = cognit_frontend_cli_get_ecf_address(&pt_dr_sm->cfc, pt_dr_sm->biscuit_token, pt_dr_sm->app_req_id);
    if (ret == 0)
    {
        COGNIT_LOG_DEBUG("ECF address obtained");
        if (pt_dr_sm->m_t_config.local_endpoint[0] == '\0')
        {
            ecf_cli_init(&pt_dr_sm->ecf, pt_dr_sm->cfc.ecf_resp.template);
        }
        else
        {
            ecf_cli_init(&pt_dr_sm->ecf, pt_dr_sm->m_t_config.local_endpoint);
        }
    }
    else
    {
        COGNIT_LOG_ERROR("Could not obtain ECF address");
    }
}

static void init_action(device_runtime_sm_t* pt_dr_sm)
{

    pt_dr_sm->up_req_counter      = 0;
    pt_dr_sm->get_address_counter = 0;

    cognit_frontend_cli_init(&pt_dr_sm->cfc, &pt_dr_sm->m_t_config);
    cognit_frontend_cli_authenticate(&pt_dr_sm->cfc, pt_dr_sm->biscuit_token);

    COGNIT_LOG_DEBUG("Token: %s", pt_dr_sm->biscuit_token);
}

static void ready_action(device_runtime_sm_t* pt_dr_sm)
{
    COGNIT_LOG_DEBUG("Entering READY state");
    pt_dr_sm->get_address_counter = 0;
}

static void send_init_request_action(device_runtime_sm_t* pt_dr_sm)
{
    COGNIT_LOG_DEBUG("Entering INIT REQUEST state");
    COGNIT_LOG_DEBUG("Uploading requirements");
    pt_dr_sm->up_req_counter++;
    int ret = cognit_frontend_cli_update_requirements(&pt_dr_sm->cfc, pt_dr_sm->biscuit_token, pt_dr_sm->m_t_requirements, &pt_dr_sm->app_req_id);
    if (ret == 0)
    {
        pt_dr_sm->requirements_uploaded = true;
    }
    else
    {
        pt_dr_sm->requirements_uploaded = false;
    }
}

static void execute_action(device_runtime_sm_t* pt_dr_sm)
{

    switch (pt_dr_sm->current_state)
    {
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
            COGNIT_LOG_DEBUG("Current state: READY");
            ready_action(pt_dr_sm);
            break;
        default:
            COGNIT_LOG_ERROR("Invalid state");
    }
}

// Function to get the next state based on the current state and event
int dr_state_machine_execute_transition(device_runtime_sm_t* pt_dr_sm, Event_t event)
{
    for (int i = 0; i < sizeof(transitions) / sizeof(sm_transition_t); i++)
    {
        if (transitions[i].previous_state == pt_dr_sm->current_state && transitions[i].event == event)
        {
            int ret = transitions[i].check_conditions(pt_dr_sm);
            if (ret)
            {
                pt_dr_sm->current_state = transitions[i].next_state;
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

static bool check_reqs(const scheduling_t* old_reqs, const scheduling_t* new_reqs)
{
    if (new_reqs == NULL)
    {
        // Si uno es NULL y el otro no, no son iguales;
        // si ambos son NULL, son "iguales" en el sentido de punteros, pero no hay dato que comparar.
        // Definir esta lógica depende de tu criterio.
        COGNIT_LOG_ERROR("Requirements are NULL");
        return false;
    }

    // Comparamos cada campo:
    if (strcmp(old_reqs->flavour, new_reqs->flavour) != 0)
    {
        return true;
    }
    if (old_reqs->max_latency != new_reqs->max_latency)
    {
        return true;
    }
    if (old_reqs->max_function_execution_time != new_reqs->max_function_execution_time)
    {
        return true;
    }
    if (old_reqs->min_renewable != new_reqs->min_renewable)
    {
        return true;
    }
    if (strcmp(old_reqs->geolocation, new_reqs->geolocation) != 0)
    {
        return true;
    }

    COGNIT_LOG_INFO("New requirements are equal to old requirements");
    return false;
}

e_status_code_t dr_sm_update_requirements(device_runtime_sm_t* pt_dr_sm, scheduling_t t_reqs)
{
    if (!check_reqs(&pt_dr_sm->m_t_requirements, &t_reqs))
    {
        return E_ST_CODE_SUCCESS;
    }
    pt_dr_sm->requirements_changed = true;
    pt_dr_sm->m_t_requirements     = t_reqs;
    COGNIT_LOG_INFO("Requirements have changed! Applying...");
    pt_dr_sm->requirements_uploaded = false;

    if (!cfc_get_has_connection(&pt_dr_sm->cfc))
    {
        COGNIT_LOG_ERROR("Frontend client is not connected: requirements could not be uploaded.");
        memset(&pt_dr_sm->m_t_requirements, 0, sizeof(scheduling_t));
        if (pt_dr_sm->current_state == INIT)
        {
            dr_state_machine_execute_transition(pt_dr_sm, TOKEN_NOT_VALID_READY);
        }
        else if (pt_dr_sm->current_state == GET_ECF_ADDRESS)
        {
            dr_state_machine_execute_transition(pt_dr_sm, TOKEN_NOT_VALID_ADDRESS);
        }
        else if (pt_dr_sm->current_state == SEND_INIT_REQUEST)
        {
            dr_state_machine_execute_transition(pt_dr_sm, TOKEN_NOT_VALID_REQUIREMENTS);
        }

        return E_ST_CODE_ERROR;
    }

    if (pt_dr_sm->current_state == SEND_INIT_REQUEST)
    {
        dr_state_machine_execute_transition(pt_dr_sm, SEND_INIT_UPDATE_REQUIREMENTS);
    }

    if (pt_dr_sm->current_state == INIT)
    {
        dr_state_machine_execute_transition(pt_dr_sm, SUCCESS_AUTH);
    }

    if (pt_dr_sm->current_state == READY)
    {
        pt_dr_sm->up_req_counter = 0;
        dr_state_machine_execute_transition(pt_dr_sm, READY_UPDATE_REQUIREMENTS);
    }

    if (pt_dr_sm->current_state == GET_ECF_ADDRESS)
    {
        dr_state_machine_execute_transition(pt_dr_sm, ADDRESS_UPDATE_REQUIREMENTS);
    }

    while (!pt_dr_sm->requirements_uploaded)
    {
        COGNIT_LOG_INFO("Counter: %d\n", pt_dr_sm->up_req_counter);

        COGNIT_LOG_ERROR("Counter: %d\n", pt_dr_sm->up_req_counter);
        COGNIT_LOG_ERROR("Requirements upload failed. Retrying...");
        if (pt_dr_sm->up_req_counter >= MAX_REQ_UPLOAD_ATTEMPTS)
        {
            COGNIT_LOG_ERROR("Number of attempts reached: unable to upload requirements. State machine is now in init state.");
            memset(&pt_dr_sm->m_t_requirements, 0, sizeof(scheduling_t));
            dr_state_machine_execute_transition(pt_dr_sm, LIMIT_REQUIREMENTS_UPLOAD);
            pt_dr_sm->requirements_changed = false;
            return E_ST_CODE_ERROR;
        }
        dr_state_machine_execute_transition(pt_dr_sm, RETRY_REQUIREMENTS_UPLOAD);
    }

    COGNIT_LOG_INFO("Requirements succesfully uploaded! Entering GET_ECF_ADDRESS state...");
    pt_dr_sm->requirements_changed = false;
    dr_state_machine_execute_transition(pt_dr_sm, REQUIREMENTS_UP);

    return E_ST_CODE_SUCCESS;
}

int dr_state_machine_init(device_runtime_sm_t* pt_dr_sm, cognit_config_t t_config, faas_t* pt_faas)
{
    if (pt_dr_sm == NULL)
    {
        COGNIT_LOG_ERROR("Device runtime state machine not initialized");
        return -1;
    }

    memset(pt_dr_sm, 0, sizeof(device_runtime_sm_t));

    pt_dr_sm->current_state = INIT;
    pt_dr_sm->m_t_config    = t_config;

    proto_parser_init(pt_faas);

    COGNIT_LOG_DEBUG("Starting state machine")
    execute_action(pt_dr_sm);

    return 0;
}

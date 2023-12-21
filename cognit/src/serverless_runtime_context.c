#include "serverless_runtime_context.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <ip_utils.h>

// Private functions
static char* serialize_requirements(energy_scheduling_policy_t t_energy_scheduling_policy)
{
    cJSON* json = cJSON_CreateObject();

    cJSON_AddNumberToObject(json, t_energy_scheduling_policy.c_policy_name, t_energy_scheduling_policy.ui32_energy_percentage);
    char* json_string = cJSON_Print(json);
    // Free JSON obj (not the string)
    cJSON_Delete(json);

    return json_string;
}

static void init_xaas_config(xaas_config_t* t_xaas_config)
{
    // Initialize the values in the structure
    t_xaas_config->ui8_cpu        = 1;
    t_xaas_config->ui32_memory    = 768;
    t_xaas_config->ui32_disk_size = 3072;
    strcpy(t_xaas_config->c_flavour, "nature");
    strcpy(t_xaas_config->c_endpoint, "");
    strcpy(t_xaas_config->c_state, "");
    strcpy(t_xaas_config->c_vm_id, "");
}

// Public functions
e_status_code_t init_serverless_runtime_context(cognit_config_t* config)
{
    e_status_code_t e_ret = E_ST_CODE_SUCCESS;
    uint8_t ui8_pe_ret    = PE_ERR_CODE_SUCCESS;

    m_t_serverless_runtime_context.m_t_prov_eng_context.m_cognit_prov_engine_config = *config;
    snprintf(m_t_serverless_runtime_context.m_c_url_proto, sizeof(m_t_serverless_runtime_context.m_c_url_proto), "%s", "http://");
    ui8_pe_ret = prov_eng_init(&m_t_serverless_runtime_context.m_t_prov_eng_context);

    if (ui8_pe_ret != PE_ERR_CODE_SUCCESS)
    {
        e_ret = E_ST_CODE_ERROR;
    }
    // TODO: add sr_instance, src?

    return e_ret;
}

e_status_code_t create_serverless_runtime(basic_serverless_runtime_conf_t t_basic_serverless_runtime_conf)
{
    serverless_runtime_conf_t t_sr_conf_pe = { 0 };
    char c_policies[SR_POLICY_MAX_LEN]     = "";
    char c_requirements[SR_REQ_MAX_LEN]    = "";

    int i_ret = 0;

    for (int i = 0; i < MAX_SCHEDULING_POLICIES; i++)
    {
        strcat(c_policies, t_basic_serverless_runtime_conf.scheduling_policies[i].c_policy_name);
        char* serialized_req_element = serialize_requirements(t_basic_serverless_runtime_conf.scheduling_policies[i]);
        strcat(c_requirements, serialized_req_element);
        free(serialized_req_element);

        // Add comma only if it's not the last element
        if (i != MAX_SCHEDULING_POLICIES - 1)
        {
            strcat(c_policies, ",");
            strcat(c_requirements, ",");
        }
    }

    // Copy flavour to faas_config
    strncpy(m_t_serverless_runtime_context.m_t_prov_eng_context.m_serverless_runtime_conf.faas_config.c_flavour, t_basic_serverless_runtime_conf.name, sizeof(m_t_serverless_runtime_context.m_t_prov_eng_context.m_serverless_runtime_conf.faas_config.c_flavour));
    // Load default values to faas_config
    init_xaas_config(&m_t_serverless_runtime_context.m_t_prov_eng_context.m_serverless_runtime_conf.faas_config);
    // Copy policies and requirements to serverless_runtime_conf
    strncpy(m_t_serverless_runtime_context.m_t_prov_eng_context.m_serverless_runtime_conf.scheduling_config.c_policy, c_policies, sizeof(m_t_serverless_runtime_context.m_t_prov_eng_context.m_serverless_runtime_conf.scheduling_config.c_policy));
    strncpy(m_t_serverless_runtime_context.m_t_prov_eng_context.m_serverless_runtime_conf.scheduling_config.c_requirements, c_requirements, sizeof(m_t_serverless_runtime_context.m_t_prov_eng_context.m_serverless_runtime_conf.scheduling_config.c_requirements));
    // Load device info empty TODO: add device info
    m_t_serverless_runtime_context.m_t_prov_eng_context.m_serverless_runtime_conf.device_info = (device_info_t) { 0 };

    // Create serverless runtime
    t_sr_conf_pe = prov_eng_create_runtime(&m_t_serverless_runtime_context.m_t_prov_eng_context.m_serverless_runtime_conf);

    // if t_sr_conf_pe is == {0} then there was an error
    if (memcmp(&t_sr_conf_pe, &(serverless_runtime_conf_t) { 0 }, sizeof(serverless_runtime_conf_t)) == 0)
    {
        fprintf(stderr, "[sr_context] Serverless Runtime creation request failed\n");
        return E_ST_CODE_ERROR;
    }

    if (t_sr_conf_pe.faas_config.c_state != FaaSState_RUNNING)
    {
        fprintf(stderr, "[sr_context] Serverless Runtime creation request failed: returned state is not PENDING, is %s\n", t_sr_conf_pe.faas_config.c_state);
        return E_ST_CODE_ERROR;
    }

    i_ret = get_ip_version(t_sr_conf_pe.faas_config.c_endpoint);

    if (i_ret == 0)
    {
        fprintf(stderr, "[sr_context] Serverless Runtime creation request failed: returned endpoint IP is not valid\n");
        return E_ST_CODE_ERROR;
    }
    else if (i_ret == IP_V4)
    {
        snprintf(m_t_serverless_runtime_context.m_c_endpoint, sizeof(m_t_serverless_runtime_context.m_c_endpoint), "%s%s:%d", m_t_serverless_runtime_context.m_c_url_proto, t_sr_conf_pe.faas_config.c_endpoint, m_t_serverless_runtime_context.m_t_prov_eng_context.m_cognit_prov_engine_config.ui32_serv_runtime_port);
        printf("[sr_context] Serverless Runtime created successfully, endpoint: %s\n", m_t_serverless_runtime_context.m_c_endpoint);
    }
    else if (i_ret == IP_V6)
    {
        snprintf(m_t_serverless_runtime_context.m_c_endpoint, sizeof(m_t_serverless_runtime_context.m_c_endpoint), "%s[%s]:%d", m_t_serverless_runtime_context.m_c_url_proto, t_sr_conf_pe.faas_config.c_endpoint, m_t_serverless_runtime_context.m_t_prov_eng_context.m_cognit_prov_engine_config.ui32_serv_runtime_port);
        printf("[sr_context] Serverless Runtime created successfully, endpoint: %s\n", m_t_serverless_runtime_context.m_c_endpoint);
    }

    // Copy the serverless runtime config to the context
    m_t_serverless_runtime_context.m_t_prov_eng_context.m_serverless_runtime_conf = t_sr_conf_pe;

    return SUCCESS;
}

const char* check_serverless_runtime_status(uint32_t ui32_serverless_runtime_id)
{
    // Implementa la lógica aquí
    return "PENDING";
}

exec_response_t call_sync(exec_faas_params_t* t_faas_params)
{
    // Implementa la lógica aquí
}

async_exec_response_t call_async(exec_faas_params_t* t_faas_params)
{
    // Implementa la lógica aquí
}

const char* wait_for_task(const char* c_async_task_id, uint32_t ui32_timeout_ms)
{
    // Implementa la lógica aquí
    return "PENDING";
}

void delete_serverless_runtime(const char* c_endpoint)
{
    // Implementa la lógica aquí
}
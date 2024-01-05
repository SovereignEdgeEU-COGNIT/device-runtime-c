#include "serverless_runtime_context.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <ip_utils.h>
#include <unistd.h>

// Private functions
static int serialize_energy_requirements(energy_scheduling_policy_t t_energy_scheduling_policy, char* pch_serialized_energy_requirements, const size_t ui32_serialized_energy_requirements_len)
{
    cJSON* pt_json  = cJSON_CreateObject();
    int i_json_size = 0;

    cJSON_AddNumberToObject(pt_json, "energy", t_energy_scheduling_policy.ui32_energy_percentage);
    char* pch_json_string = cJSON_Print(pt_json);

    i_json_size = strlen(pch_json_string);

    if (i_json_size > ui32_serialized_energy_requirements_len)
    {
        COGNIT_LOG_ERROR("[sr_context] serialize_energy_requirements: serialized string is too big, %d bytes, max allowed %d bytes\n", i_json_size, ui32_serialized_energy_requirements_len);
        return -1;
    }

    memcpy(pch_serialized_energy_requirements, pch_json_string, i_json_size);

    cJSON_Delete(pt_json);
    free(pch_json_string);

    return 0;
}

static void init_faas_config(faas_config_t* t_xaas_config)
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
e_status_code_t serverless_runtime_ctx_init(serverless_runtime_context_t* pt_sr_ctx, const cognit_config_t* pt_cfg)
{
    e_status_code_t e_ret = E_ST_CODE_ERROR;

    // Initial cleanup
    memset(pt_sr_ctx, 0, sizeof(serverless_runtime_context_t));

    // Copy the provided config into the sr instance
    memcpy(&pt_sr_ctx->m_t_cognit_conf, pt_cfg, sizeof(cognit_config_t));

    // Init the provisioning engine client using the config
    if (prov_engine_cli_init(&pt_sr_ctx->m_t_prov_engine_cli, &pt_sr_ctx->m_t_cognit_conf) == PE_ERR_CODE_SUCCESS)
    {
        e_ret = E_ST_CODE_SUCCESS;
    }

    return e_ret;
}

e_status_code_t serverless_runtime_ctx_create(serverless_runtime_context_t* pt_sr_ctx, const serverless_runtime_conf_t* pt_sr_conf)
{

    // Load default values to faas_config
    init_faas_config(&pt_sr_ctx->m_t_serverless_runtime.faas_config);

    // For now ignore the provided flavour and use the default one
    // Only copy the name
    strncpy(pt_sr_ctx->m_t_serverless_runtime.c_name, pt_sr_conf->name, sizeof(pt_sr_ctx->m_t_serverless_runtime.c_name));

    // TODO: add device info

    // TODO Add policies and requirements

    // Serialize energy requirements
    // for (int i = 0; i < MAX_ENERGY_SCHEDULING_POLICIES; i++)
    // {
    //     serialize_energy_requirements(pt_sr_conf.m_t_energy_scheduling_policies[i],pt_sr_ctx->m_t_prov_eng_context.m_serverless_runtime_conf.m_t_energy_scheduling_policies[i], sizeof(pt_sr_ctx->m_t_serverless_runtime.scheduling_config.
    // }

    // Create serverless runtime
    if (prov_engine_cli_create_runtime(&pt_sr_ctx->m_t_prov_engine_cli, &pt_sr_ctx->m_t_serverless_runtime) != COGNIT_ECODE_SUCCESS)
    {
        COGNIT_LOG_ERROR("[sr_context] Serverless Runtime creation request failed\n");
        return E_ST_CODE_ERROR;
    }

    // Check the state returned by the provisioning engine i
    if (pt_sr_ctx->m_t_serverless_runtime.faas_config.c_state != STR_FAAS_STATE_PENDING || pt_sr_ctx->m_t_serverless_runtime.faas_config.c_state != STR_FAAS_STATE_NO_STATE)
    {
        COGNIT_LOG_ERROR("[sr_context] Serverless Runtime creation request failed: returned state is not PENDING, is %s\n", t_sr_conf_pe.faas_config.c_state);
        return COGNIT_ECODE_ERROR;
    }

    COGNIT_LOG_INFO("[sr_context] Serverless Runtime create request completed successfully\n");

    return COGNIT_ECODE_ERROR;
}

e_faas_state_t serverless_runtime_ctx_status(serverless_runtime_context_t* pt_sr_ctx)
{
    int i_ret = COGNIT_ECODE_ERROR;

    // Check if the serverless runtime instance was created and already has an ID
    if (pt_sr_ctx == 0 || pt_sr_ctx->m_t_serverless_runtime.ui32_id == 0)
    {
        COGNIT_LOG_ERROR("[sr_context] Serverless Runtime not created yet\n");
        return E_FAAS_STATE_ERROR;
    }

    // Retrieve the serverless runtime from the provisioning engine using the ID
    i_ret = prov_engine_cli_retreive_runtime(&pt_sr_ctx->m_t_prov_engine_cli, pt_sr_ctx->m_t_serverless_runtime.ui32_id, &pt_sr_ctx->m_t_serverless_runtime);

    if (i_ret != COGNIT_ECODE_SUCCESS)
    {
        COGNIT_LOG_ERROR("[sr_context] Serverless Runtime retrieval request failed\n");
        return E_FAAS_STATE_ERROR;
    }

    // Check the state returned by the provisioning engine
    if (strcmp(pt_sr_ctx->m_t_serverless_runtime.faas_config.c_state, STR_FAAS_STATE_PENDING) == 0)
    {
        COGNIT_LOG_INFO("[sr_context] Serverless Runtime is PENDING");
        return E_FAAS_STATE_PENDING;
    }
    else if (strcmp(pt_sr_ctx->m_t_serverless_runtime.faas_config.c_state, STR_FAAS_STATE_RUNNING) == 0)
    {
        COGNIT_LOG_INFO("[sr_context] Serverless Runtime is RUNNING");
        return E_FAAS_STATE_RUNNING;
    }
    else if (strcmp(pt_sr_ctx->m_t_serverless_runtime.faas_config.c_state, STR_FAAS_STATE_NO_STATE) == 0)
    {
        COGNIT_LOG_INFO("[sr_context] Serverless Runtime is NO_STATE");
        return E_FAAS_STATE_NO_STATE;
    }

    COGNIT_LOG_ERROR("[sr_context] Serverless Runtime retrieval request failed: returned state is not PENDING, RUNNING or NO_STATE, is %s\n", pt_sr_ctx->m_t_serverless_runtime.faas_config.c_state);
    return E_FAAS_STATE_ERROR;
}

exec_response_t srcontext_call_sync(exec_faas_params_t* exec_faas_params)
{
    exec_response_t t_exec_response;
    uint8_t ui8_off_func_data[10000];
    size_t payload_len;

    if (srcli_there_is_srv_runtime_created() == false)
    {
        if (m_t_serverless_runtime_context.m_c_endpoint == NULL
            && m_t_serverless_runtime_context.m_t_prov_eng_context.m_serverless_runtime_conf.faas_config.c_endpoint == NULL)
        {
            COGNIT_LOG_ERROR("[sr_context] Serverless Runtime not created yet\n");
            return (exec_response_t) { 0 };
        }
        else
        {
            srcli_init(m_t_serverless_runtime_context.m_c_endpoint);
        }
    }

    if (faasparser_parse_exec_faas_params_as_str_json(exec_faas_params, ui8_off_func_data, &payload_len) == JSON_ERR_CODE_OK)
    {
        COGNIT_LOG_DEBUG("Params parsed successfully, generated JSON: %s\n", ui8_off_func_data);
    }

    t_exec_response = srcli_faas_exec_sync(ui8_off_func_data, payload_len);

    return t_exec_response;
}

async_exec_response_t srcontext_call_async(exec_faas_params_t* exec_faas_params)
{
    async_exec_response_t t_async_exec_response;
    uint8_t ui8_off_func_data[10000];
    size_t payload_len;

    if (srcli_there_is_srv_runtime_created() == false)
    {
        if (m_t_serverless_runtime_context.m_c_endpoint == NULL
            && m_t_serverless_runtime_context.m_t_prov_eng_context.m_serverless_runtime_conf.faas_config.c_endpoint == NULL)
        {
            COGNIT_LOG_ERROR("[sr_context] Serverless Runtime not created yet\n");
            return (async_exec_response_t) { 0 };
        }
        else
        {
            srcli_init(m_t_serverless_runtime_context.m_c_endpoint);
        }
    }

    if (faasparser_parse_exec_faas_params_as_str_json(exec_faas_params, ui8_off_func_data, &payload_len) == JSON_ERR_CODE_OK)
    {
        COGNIT_LOG_DEBUG("Params parsed successfully, generated JSON: %s\n", ui8_off_func_data);
    }

    t_async_exec_response = srcli_faas_exec_async(ui8_off_func_data, payload_len);

    return t_async_exec_response;
}

async_exec_response_t srcontext_wait_for_task(const char* c_async_task_id, uint32_t ui32_timeout_ms)
{
    async_exec_response_t t_async_response;

    if (srcli_there_is_srv_runtime_created() == false)
    {
        if (m_t_serverless_runtime_context.m_c_endpoint == NULL
            && m_t_serverless_runtime_context.m_t_prov_eng_context.m_serverless_runtime_conf.faas_config.c_endpoint == NULL)
        {
            COGNIT_LOG_ERROR("[sr_context] Serverless Runtime not created yet\n");
            return (async_exec_response_t) { 0 };
        }
        else
        {
            srcli_init(m_t_serverless_runtime_context.m_c_endpoint);
        }
    }

    // Timeout management loop.
    while (ui32_timeout_ms > INTERVAL_1MS)
    {
        t_async_response = srcli_wait_for_task(c_async_task_id, ui32_timeout_ms);
        if (t_async_response.status == "READY")
        {
            if (t_async_response.res != NULL)
            {
                break;
            }
        }
        usleep(INTERVAL_1MS * 1000);
        ui32_timeout_ms -= INTERVAL_1MS;
    }

    return t_async_response;
}

void delete_serverless_runtime(const char* c_endpoint)
{
    if (srcli_there_is_srv_runtime_created() == false)
    {
        if (m_t_serverless_runtime_context.m_c_endpoint == NULL
            || m_t_serverless_runtime_context.m_t_prov_eng_context.m_serverless_runtime_conf.faas_config.c_endpoint == NULL
            || m_t_serverless_runtime_context.m_t_prov_eng_context.m_serverless_runtime_conf.ui32_id == 0)
        {
            COGNIT_LOG_ERROR("[sr_context] Serverless Runtime not created yet\n");
            return;
        }
        else
        {
            if (prov_engine_delete_runtime(m_t_serverless_runtime_context.m_t_prov_eng_context.m_serverless_runtime_conf.ui32_id) == true)
            {
                COGNIT_LOG_INFO("[sr_context] Serverless Runtime deleted successfully\n");
            }
            else
            {
                COGNIT_LOG_ERROR("[sr_context] Serverless Runtime deletion failed\n");
            }
        }
    }
}
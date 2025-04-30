#include <device_runtime.h>
#include <string.h>
#include <logger.h>

static void free_config(cognit_config_t *config)
{
    free((void *)config->cognit_frontend_endpoint);
    free((void *)config->cognit_frontend_usr);
    free((void *)config->cognit_frontend_pwd);
}

e_status_code_t device_runtime_init(device_runtime_t* pt_dr, cognit_config_t t_config, scheduling_t t_reqs, faas_t *pt_faas)
{
    if(pt_dr == NULL)
    {
        COGNIT_LOG_ERROR("Device runtime not initialized");
        return E_ST_CODE_ERROR;
    }

    memset(pt_dr, 0, sizeof(device_runtime_t));

    pt_dr->m_t_config = t_config;

    int ret = dr_state_machine_init(&pt_dr->m_t_device_runtime_sm, pt_dr->m_t_config, pt_faas);

    dr_sm_update_requirements(&pt_dr->m_t_device_runtime_sm, t_reqs);
    
    return ret;
}


e_status_code_t device_runtime_call(device_runtime_t* pt_dr, faas_t *pt_faas, scheduling_t t_new_reqs, void** pt_exec_response)
{
    if(pt_dr == NULL)
    {
        COGNIT_LOG_ERROR("Device runtime state machine not initialized");
        return E_ST_CODE_ERROR;
    }

    dr_sm_update_requirements(&pt_dr->m_t_device_runtime_sm, t_new_reqs);

    dr_sm_offload_function(&pt_dr->m_t_device_runtime_sm, pt_faas, pt_exec_response);
    //Handle response
    if(*pt_exec_response == NULL)
    {
        COGNIT_LOG_ERROR("Response not obtailed");
        return E_ST_CODE_ERROR;
    }
    
    return E_ST_CODE_SUCCESS;
}







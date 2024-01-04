#include <prov_engine_cli.h>
#include <string.h>

static prov_eng_cli_context_t* m_t_pec_context;

int8_t pec_init(prov_eng_cli_context_t* t_pec_context)
{
    int8_t i8_ret = PE_ERR_CODE_SUCCESS;

    m_t_pec_context = &t_pec_context;
    // TODO: validate config

    return i8_ret;
}

serverless_runtime_conf_t pec_create_runtime(serverless_runtime_conf_t* serverless_runtime_context)
{
    serverless_runtime_conf_t t_ret;
    return t_ret;
}
serverless_runtime_conf_t pec_retreive_runtime(uint32_t ui32_id)
{
    serverless_runtime_conf_t t_ret;
    return t_ret;
}
bool pec_delete_runtime(uint32_t ui32_id)
{
    bool b_ret = false;
    return b_ret;
}
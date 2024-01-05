#include <prov_engine_cli.h>

int prov_engine_cli_init(prov_engine_cli_t* t_prov_engine_cli, const cognit_config_t* pt_cognit_config)
{
    // Setup the instance to point to the configuration
    t_prov_engine_cli->m_t_config = pt_cognit_config;
    return 0;
}

int prov_engine_cli_create_runtime(prov_engine_cli_t* t_prov_engine_cli, serverless_runtime_t* t_serverless_runtime)
{
    // TODO: Implement this function
    return -1;
}

int prov_engine_cli_retreive_runtime(prov_engine_cli_t* t_prov_engine_cli, uint32_t ui32_id, serverless_runtime_t* t_serverless_runtime)
{
    // TODO: Implement this function
    return -1;
}

int prov_engine_delete_runtime(prov_engine_cli_t* t_prov_engine_cli, uint32_t ui32_id, serverless_runtime_t* t_serverless_runtime)
{
    // TODO: Implement this function
    return -1;
}
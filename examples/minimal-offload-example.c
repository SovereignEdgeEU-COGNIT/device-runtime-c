

// TODO

#include <stdio.h>
#include "cognit_http.h"
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <serverless_runtime_context.h>
#include <unistd.h>

int8_t my_http_send_req_cb(const char* c_buffer, size_t size, http_config_t* config)
{
    printf("SEND HERE");
}

int main(int argc, char const* argv[])
{
    cognit_config_t t_my_cognit_config;
    serverless_runtime_context_t t_my_serverless_runtime_context;
    serverless_runtime_conf_t t_my_serverless_runtime_conf;

    // Initialize the config for the serverless runtime context instance
    t_my_cognit_config.prov_engine_endpoint   = "http://localhost:8080";
    t_my_cognit_config.prov_engine_pe_usr     = "admin";
    t_my_cognit_config.prov_engine_pe_pwd     = "admin";
    t_my_cognit_config.prov_engine_port       = 8080;
    t_my_cognit_config.ui32_serv_runtime_port = 8080;

    serverless_runtime_ctx_init(&t_my_serverless_runtime_context, &t_my_cognit_config);

    // Cofigure the initial serverless runtime requirements
    t_my_serverless_runtime_conf.name = "my_serverless_runtime";

    if (serverless_runtime_ctx_create(&t_my_serverless_runtime_context, &t_my_serverless_runtime_conf) != E_ST_CODE_SUCCESS)
    {
        printf("Error configuring serverless runtime\n");
        return -1;
    }

    // Check the serverless runtime status

    while (true)
    {
        if (serverless_runtime_ctx_status(&t_my_serverless_runtime_context) == E_FAAS_STATE_RUNNING)
        {
            printf("Serverless runtime is ready\n");
            break;
        }

        printf("Serverless runtime is not ready\n");

        sleep(1);
    }

    // Offload the function exection to the serverless runtime
    // This will use the callback function my_http_send_req to send the request

    // TODO: Create MACROS to define easly the exec_faas_params_t structure and obtain the result
    serverless_runtime_ctx_call_sync(&t_my_serverless_runtime_context, 0, 0);

    return 0;
}

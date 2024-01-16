

// TODO

#include <stdio.h>
#include "cognit_http.h"
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <serverless_runtime_context.h>
#include <unistd.h>
#include <offload_fc_c.h>
#include <faas_parser.h>

FUNC_TO_STR(
    mult_fc,
    void mult(int a, int b, float* c) {
        *c = a * b;
    })

int8_t my_http_send_req_cb(const char* c_buffer, size_t size, http_config_t* config)
{
    printf("SEND HERE");
}

int main(int argc, char const* argv[])
{
    cognit_config_t t_my_cognit_config;
    serverless_runtime_context_t t_my_serverless_runtime_context;
    serverless_runtime_conf_t t_my_serverless_runtime_conf;
    exec_response_t t_exec_response;
    exec_faas_params_t exec_params = { 0 };

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

    const char* includes = INCLUDE_HEADERS(#include<stdio.h> \n);
    offload_fc_c_create(&exec_params, includes, mult_fc_str);
    // Param 1
    offload_fc_c_add_param(&exec_params, "a", "IN");
    offload_fc_c_set_param(&exec_params, "int", "3");
    // Param 2
    offload_fc_c_add_param(&exec_params, "b", "IN");
    offload_fc_c_set_param(&exec_params, "int", "4");
    // Param 3
    offload_fc_c_add_param(&exec_params, "c", "OUT");
    offload_fc_c_set_param(&exec_params, "float", NULL);

    serverless_runtime_ctx_call_sync(&t_my_serverless_runtime_context, &exec_params, &t_exec_response);


    // TODO: wrap all destroys in a single function
    faasparser_destroy_exec_response(&t_exec_response);

    return 0;
}

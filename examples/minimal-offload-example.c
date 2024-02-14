#include <stdio.h>
#include "cognit_http.h"
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <serverless_runtime_context.h>
#include <unistd.h>
#include <offload_fc_c.h>
#include <faas_parser.h>
#include <cognit_http.h>
#include <logger.h>
#include <ip_utils.h>

FUNC_TO_STR(
    mult_fc,
    void mult(int a, int b, float* c) {
        *c = a * b;
    })

size_t handle_response_data_cb(void* data_content, size_t size, size_t nmemb, void* user_buffer)
{
    size_t realsize           = size * nmemb;
    http_response_t* response = (http_response_t*)user_buffer;

    if (response->size + realsize >= sizeof(response->ui8_response_data_buffer))
    {
        COGNIT_LOG_ERROR("Response buffer too small");
        return 0;
    }

    memcpy(&(response->ui8_response_data_buffer[response->size]), data_content, realsize);
    response->size += realsize;
    response->ui8_response_data_buffer[response->size] = '\0';

    return realsize;
}

int my_http_send_req_cb(const char* c_buffer, size_t size, http_config_t* config)
{
    CURL* curl;
    CURLcode res;
    long http_code             = 0;
    struct curl_slist* headers = NULL;
    memset(&config->t_http_response.ui8_response_data_buffer, 0, sizeof(config->t_http_response.ui8_response_data_buffer));
    config->t_http_response.size = 0;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (curl)
    {
        // Set the request header
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "charset: utf-8");

        if (curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers) != CURLE_OK
            // Configure URL and payload
            || curl_easy_setopt(curl, CURLOPT_URL, config->c_url) != CURLE_OK
            // Set the callback function to handle the response data
            || curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&config->t_http_response) != CURLE_OK
            || curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handle_response_data_cb) != CURLE_OK
            || curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, config->ui32_timeout_ms) != CURLE_OK)
        {
            COGNIT_LOG_ERROR("[hhtp_send_req_cb] curl_easy_setopt() failed");
            return -1;
        }

        // Find '[' or ']' in the URL to determine the IP version
        // TODO: fix ip_utils to obtain http://[2001:67c:22b8:1::d]:8000/v1/faas/execute-sync
        // as IP_V6
        if (strchr(config->c_url, '[') != NULL
            && strchr(config->c_url, ']') != NULL)
        {
            if (curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V6) != CURLE_OK)
            {
                COGNIT_LOG_ERROR("[hhtp_send_req_cb] curl_easy_setopt()->IPRESOLVE_V6 failed");
                return -1;
            }
        }

        if (strcmp(config->c_method, HTTP_METHOD_GET) == 0)
        {
            if (curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_USERNAME, config->c_username) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_PASSWORD, config->c_password) != CURLE_OK)
            {
                COGNIT_LOG_ERROR("[hhtp_send_req_cb] curl_easy_setopt()->get() failed");
                return -1;
            }
        }
        else if (strcmp(config->c_method, HTTP_METHOD_POST) == 0)
        {
            if (curl_easy_setopt(curl, CURLOPT_POST, 1L) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST") != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, size) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_POSTFIELDS, c_buffer) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_USERNAME, config->c_username) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_PASSWORD, config->c_password) != CURLE_OK)
            {
                COGNIT_LOG_ERROR("[hhtp_send_req_cb] curl_easy_setopt()->post() failed");
                return -1;
            }
        }
        else if (strcmp(config->c_method, HTTP_METHOD_DELETE) == 0)
        {
            if (curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE") != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_USERNAME, config->c_username) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_PASSWORD, config->c_password) != CURLE_OK)
            {
                COGNIT_LOG_ERROR("[hhtp_send_req_cb] curl_easy_setopt()->post() failed");
                return -1;
            }
        }
        else
        {
            COGNIT_LOG_ERROR("[hhtp_send_req_cb] Invalid HTTP method");
            return -1;
        }

        // Make the request
        res = curl_easy_perform(curl);

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        COGNIT_LOG_ERROR("HTTP err code %ld ", http_code);

        // Check errors
        if (res != CURLE_OK)
        {
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            COGNIT_LOG_ERROR("curl_easy_perform() failed: %s", curl_easy_strerror(res));
            COGNIT_LOG_ERROR("HTTP err code %ld ", http_code);
        }

        // Clean and close CURL session
        curl_easy_cleanup(curl);
    }

    config->t_http_response.l_http_code = http_code;

    // Clean global curl configuration
    curl_global_cleanup();
    free(headers);

    return (res == CURLE_OK) ? 0 : -1;
}

int main(int argc, char const* argv[])
{
    cognit_config_t t_my_cognit_config;
    serverless_runtime_context_t t_my_serverless_runtime_context;
    serverless_runtime_conf_t t_my_serverless_runtime_conf;
    exec_response_t t_exec_response;
    exec_faas_params_t exec_params = { 0 };

    // Initialize the config for the serverless runtime context instance
    t_my_cognit_config.prov_engine_endpoint   = "localhost:8080";
    t_my_cognit_config.prov_engine_pe_usr     = "admin";
    t_my_cognit_config.prov_engine_pe_pwd     = "admin";
    t_my_cognit_config.prov_engine_port       = 8080;
    t_my_cognit_config.ui32_serv_runtime_port = 8080;

    serverless_runtime_ctx_init(&t_my_serverless_runtime_context, &t_my_cognit_config);

    // Cofigure the initial serverless runtime requirements
    t_my_serverless_runtime_conf.name                                                  = "my_serverless_runtime";
    t_my_serverless_runtime_conf.faas_flavour                                          = "DC_C_version_tests";
    t_my_serverless_runtime_conf.m_t_energy_scheduling_policies.ui32_energy_percentage = 50;

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

    COGNIT_LOG_INFO("Result: %s", t_exec_response.res_payload);

    // Free the resources
    faasparser_destroy_exec_response(&t_exec_response);
    offload_fc_c_destroy(&exec_params);

    COGNIT_LOG_INFO("Deleting serverless runtime");

    while (prov_engine_delete_runtime(&t_my_serverless_runtime_context.m_t_prov_engine_cli, t_my_serverless_runtime_context.m_t_serverless_runtime.ui32_id, &t_my_serverless_runtime_context.m_t_serverless_runtime) != 0)
    {
        COGNIT_LOG_ERROR("Error deleting serverless runtime");
        sleep(1);
    }

    COGNIT_LOG_INFO("Serverless runtime deleted");

    return 0;
}

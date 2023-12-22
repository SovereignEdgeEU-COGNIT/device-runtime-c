#include "serverless_runtime_client.h"
#include <cognit_http.h>
#include <cJSON.h>
#include <stdlib.h>
#include <string.h>
#include <faas_parser.h>

static char* m_c_endpoint;

void init_serverless_runtime_cli(const char* c_endpoint)
{
    snprintf(m_c_endpoint, 256, "%s", c_endpoint);
    printf("Serverless runtime endpoint: %s\n", m_c_endpoint);
}

exec_response_t faas_exec_sync(uint8_t* ui8_payload, size_t payload_len)
{
    int8_t i8_ret = 0;
    exec_response_t t_exec_response;
    http_config_t t_http_config;
    char c_exec_sync_url[256];
    char c_buffer[2000] = { 0 };
    snprintf(c_exec_sync_url, 256, "%s/%s/%s", m_c_endpoint, FAAS_VERSION, FAAS_EXECUTE_SYNC_ENDPOINT);

    t_http_config.c_url           = c_exec_sync_url;
    t_http_config.c_method        = HTTP_METHOD_POST;
    t_http_config.ui32_timeout_ms = 10000;

    i8_ret = cognit_http_send(ui8_payload, payload_len, &t_http_config);
    printf("FaaS execute sync [POST¯URL]: %s\n", c_exec_sync_url);

    if (i8_ret != 0
        || t_http_config.t_http_response.ui8_response_data_buffer == NULL
        || t_http_config.t_http_response.size == 0
        || t_http_config.t_http_response.l_http_code != 200)
    {
        printf("Error sending HTTP request, HTTP code: %d\n", i8_ret);
        t_exec_response.ret_code = ERROR;
    }
    else
    {
        // Print json response
        printf("JSON received from serverless runtime: %s\n", t_http_config.t_http_response.ui8_response_data_buffer);
        printf("JSON received size: %ld\n", t_http_config.t_http_response.size);
        snprintf(c_buffer, 2000, "%s", t_http_config.t_http_response.ui8_response_data_buffer);

        // Copy the response json to the response struct
        i8_ret = parse_json_str_as_exec_faas_params(c_buffer, &t_exec_response);
        
        if (i8_ret != 0)
        {
            printf("Error parsing JSON\n");
            t_exec_response.ret_code = ERROR;
        }
    }

    free(t_http_config.t_http_response.ui8_response_data_buffer);
    t_exec_response.http_err_code = t_http_config.t_http_response.l_http_code;

    return t_exec_response;
}

async_exec_response_t faas_exec_async(uint8_t* ui8_payload, size_t payload_len)
{
    async_exec_response_t t_async_exec_response;
    int8_t i8_ret = 0;
    http_config_t t_http_config;
    char c_exec_async_url[256];
    char c_buffer[2000] = { 0 };
    snprintf(c_exec_async_url, 256, "%s/%s/%s", m_c_endpoint, FAAS_VERSION, FAAS_EXECUTE_ASYNC_ENDPOINT);

    t_http_config.c_url           = c_exec_async_url;
    t_http_config.c_method        = HTTP_METHOD_POST;
    t_http_config.ui32_timeout_ms = 10000;

    i8_ret = cognit_http_send(ui8_payload, payload_len, &t_http_config);
    printf("FaaS execute async [POST¯URL]: %s\n", c_exec_async_url);

    if (i8_ret != 0
        || t_http_config.t_http_response.ui8_response_data_buffer == NULL
        || t_http_config.t_http_response.size == 0)
    {
        printf("Error sending HTTP request, HTTP code: %d\n", i8_ret);
        t_async_exec_response.res->ret_code = ERROR;
    }
    else
    {
        // Print json response
        printf("JSON received from serverless runtime: %s\n", t_http_config.t_http_response.ui8_response_data_buffer);
        printf("JSON received size: %ld\n", t_http_config.t_http_response.size);
        snprintf(c_buffer, 2000, "%s", t_http_config.t_http_response.ui8_response_data_buffer);

        if (t_http_config.t_http_response.l_http_code == 200)
        {
            // Copy the response json to the response struct
            i8_ret = parse_json_str_as_exec_faas_params(c_buffer, &t_async_exec_response.res);
        
            if (i8_ret != 0)
            {
                printf("Error parsing JSON\n");
                t_async_exec_response.res->ret_code = ERROR;
            }
        }
        else if (t_http_config.t_http_response.l_http_code == 400)
        {
            t_async_exec_response.status                 = "FAILED";
            t_async_exec_response.res->ret_code          = ERROR;
            t_async_exec_response.exec_id.faas_task_uuid = "000-000-000";
        }
        else
        {
            t_async_exec_response.status                 = "READY";
            t_async_exec_response.res->ret_code          = ERROR;
            t_async_exec_response.exec_id.faas_task_uuid = "000-000-000";
        }
    }

    free(t_http_config.t_http_response.ui8_response_data_buffer);
    t_async_exec_response.res->http_err_code = t_http_config.t_http_response.l_http_code;

    return t_async_exec_response;
}

async_exec_response_t waitForTask(const char* c_async_task_id, uint32_t ui32_timeout_ms)
{
    async_exec_response_t t_async_exec_response;
    int8_t i8_ret = 0;
    http_config_t t_http_config;
    char c_faas_wait_id[256];
    char c_wait_for_task_url[256];
    char c_buffer[2000]    = { 0 };
    uint8_t ui8_payload[1] = { 0 };
    size_t payload_len     = 0;

    snprintf(c_faas_wait_id, 256, FAAS_WAIT_ENDPOINT, c_async_task_id);
    snprintf(c_wait_for_task_url, 256, "%s/%s/%s", m_c_endpoint, FAAS_VERSION, c_faas_wait_id);

    t_http_config.c_url           = c_wait_for_task_url;
    t_http_config.c_method        = HTTP_METHOD_GET;
    t_http_config.ui32_timeout_ms = 10000;

    i8_ret = cognit_http_send(ui8_payload, payload_len, &t_http_config);
    printf("FaaS wait [GET URL]: %s\n", c_wait_for_task_url);

    if (i8_ret != 0
        || t_http_config.t_http_response.ui8_response_data_buffer == NULL
        || t_http_config.t_http_response.size == 0)
    {
        printf("Error sending HTTP request, HTTP code: %d\n", i8_ret);
        t_async_exec_response.res->ret_code = ERROR;
    }
    else
    {
        // Print json response
        printf("JSON received from serverless runtime: %s\n", t_http_config.t_http_response.ui8_response_data_buffer);
        printf("JSON received size: %ld\n", t_http_config.t_http_response.size);
        snprintf(c_buffer, 2000, "%s", t_http_config.t_http_response.ui8_response_data_buffer);

        if (t_http_config.t_http_response.l_http_code == (200 || 400))
        {
            cJSON* root = cJSON_Parse(c_buffer);
            if (root)
            {
                // Get status from JSON previously to fully parse it
                cJSON* status = cJSON_GetObjectItemCaseSensitive(root, "status");
                if (cJSON_IsString(status))
                {
                    strncpy(t_async_exec_response.status, status->valuestring, sizeof(t_async_exec_response.status) - 1);
                }

                cJSON_Delete(root);
            }
            // Copy the response json to the response struct
            snprintf(t_async_exec_response.res->res_payload, t_http_config.t_http_response.size, "%s", c_buffer);
        }
        else
        {
            t_async_exec_response.status                 = "READY";
            t_async_exec_response.res->ret_code          = ERROR;
            t_async_exec_response.exec_id.faas_task_uuid = "000-000-000";
        }
    }

    free(t_http_config.t_http_response.ui8_response_data_buffer);
    t_async_exec_response.res->http_err_code = t_http_config.t_http_response.l_http_code;

    return t_async_exec_response;
}
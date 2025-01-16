#include "edge_cluster_frontend_cli.h"
#include <cognit_http.h>
#include <cJSON.h>
#include <stdlib.h>
#include <string.h>
#include <logger.h>

void ecf_cli_init(edge_cluster_frontend_cli_t* pt_ecf_cli, const char* c_endpoint)
{
    memset(pt_ecf_cli, 0, sizeof(edge_cluster_frontend_cli_t));
    snprintf(pt_ecf_cli->t_ecf_endpoint, MAX_URL_LENGTH, c_endpoint);
    ecf_set_has_connection(pt_ecf_cli, true);
    //snprintf(pt_serverless_runtime_cli->c_a_exec_sync_url, MAX_URL_LENGTH, "http://[%s]:8000/%s/%s", c_endpoint, FAAS_VERSION, FAAS_EXECUTE_SYNC_ENDPOINT);
    //snprintf(pt_serverless_runtime_cli->c_a_exec_async_url, MAX_URL_LENGTH, "http://[%s]:8000/%s/%s", c_endpoint, FAAS_VERSION, FAAS_EXECUTE_ASYNC_ENDPOINT);
}

int ecf_cli_faas_exec_sync(edge_cluster_frontend_cli_t* pt_ecf_cli, int func_id, uint8_t* ui8_payload, size_t payload_len, exec_response_t* pt_exec_response)
{
    int8_t i8_ret               = 0;
    http_config_t t_http_config = { 0 };

    //Clear the response
    memset(pt_exec_response, 0, sizeof(exec_response_t));

    t_http_config.c_url           = pt_ecf_cli->t_ecf_endpoint;
    t_http_config.c_method        = HTTP_METHOD_POST;
    t_http_config.ui32_timeout_ms = 10000;

    i8_ret = cognit_http_send(ui8_payload, payload_len, &t_http_config);
    COGNIT_LOG_DEBUG("FaaS execute sync [POST-URL]: %s", t_http_config.c_url);

    if (i8_ret != 0
        || t_http_config.t_http_response.ui8_response_data_buffer == NULL
        || t_http_config.t_http_response.size == 0
        || t_http_config.t_http_response.l_http_code != 200)
    {
        COGNIT_LOG_ERROR("Error sending HTTP request, HTTP code: %d", i8_ret);
        pt_exec_response->ret_code = ERROR;
    }
    else
    {
        // Print json response
        COGNIT_LOG_DEBUG("JSON received from ECF: %s", t_http_config.t_http_response.ui8_response_data_buffer);
        COGNIT_LOG_TRACE("JSON received size: %ld", t_http_config.t_http_response.size);

        // Copy the response json to the response struct
        //i8_ret = faasparser_parse_json_str_as_exec_response(t_http_config.t_http_response.ui8_response_data_buffer, pt_exec_response);

        if (i8_ret != 0)
        {
            COGNIT_LOG_ERROR("Error parsing JSON");
            pt_exec_response->ret_code = ERROR;
        }
    }

    pt_exec_response->http_err_code = t_http_config.t_http_response.l_http_code;

    // TODO IMPOORTANT handle the free of the response buffer???

    return 0;
}

bool ecf_get_has_connection(edge_cluster_frontend_cli_t* pt_ecf_cli)
{
    return pt_ecf_cli->has_connection;
}

void ecf_set_has_connection(edge_cluster_frontend_cli_t* pt_ecf_cli, bool value)
{
    pt_ecf_cli->has_connection = value;
}
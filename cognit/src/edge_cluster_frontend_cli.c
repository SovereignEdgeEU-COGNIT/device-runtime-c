#include "edge_cluster_frontend_cli.h"
#include <cognit_http.h>
#include <cJSON.h>
#include <stdlib.h>
#include <string.h>
#include <logger.h>

void ecf_cli_init(edge_cluster_frontend_cli_t* pt_ecf_cli, char* c_endpoint)
{
    memset(pt_ecf_cli, 0, sizeof(edge_cluster_frontend_cli_t));
    snprintf(pt_ecf_cli->t_ecf_endpoint, MAX_URL_LENGTH, "%s", c_endpoint);
    ecf_set_has_connection(pt_ecf_cli, true);
}

int ecf_cli_faas_exec_sync(edge_cluster_frontend_cli_t* pt_ecf_cli, char* biscuit_token, int app_req_id, faas_t* pt_faas, void** pt_exec_response)
{
    int8_t i8_ret               = 0;
    http_config_t t_http_config = { 0 };
    uint8_t ui8_payload[1024 * 16];
    size_t payload_len;
    char url[MAX_URL_LENGTH];

    if (pt_ecf_cli == NULL)
    {
        COGNIT_LOG_ERROR("Cognit frontend not initialized");
        ecf_set_has_connection(pt_ecf_cli, false);
        return 0;
    }

    if (biscuit_token == NULL)
    {
        COGNIT_LOG_ERROR("Token not provided");
        ecf_set_has_connection(pt_ecf_cli, false);
        return 0;
    }

    memset(url, 0, sizeof(url));
    
    int url_len = snprintf(url, MAX_URL_LENGTH, "%s/%s/%d/execute?app_req_id=%d&mode=sync", pt_ecf_cli->t_ecf_endpoint, FAAS_REQUEST_ENDPOINT, pt_faas->fc_id, app_req_id);
    if (url_len < 0 || url_len >= MAX_URL_LENGTH) 
    {
        return -1;
    }
    t_http_config.c_url           = url;
    t_http_config.c_method        = HTTP_METHOD_POST;
    t_http_config.ui32_timeout_ms = pt_faas->timeout_ms > 0 ? pt_faas->timeout_ms : ECF_REQ_TIMEOUT * 1000;
    t_http_config.c_token         = biscuit_token;
    t_http_config.binary          = false;

    if(faasparser_parse_exec_faas_params_as_str_json(pt_faas, ui8_payload, &payload_len) == 0)
    {
        COGNIT_LOG_DEBUG("Params parsed successfully, generated JSON: %s", ui8_payload);
    }


    if (payload_len == 0)
    {
        return -1;
    }

    i8_ret = cognit_http_send((char*) ui8_payload, payload_len, &t_http_config);
    COGNIT_LOG_DEBUG("FaaS execute sync [POST-URL]: %s", t_http_config.c_url);

    if (i8_ret != 0
        || t_http_config.t_http_response.ui8_response_data_buffer == NULL
        || t_http_config.t_http_response.size == 0
        || t_http_config.t_http_response.l_http_code != 200)
    {
        COGNIT_LOG_ERROR("Error sending HTTP request, HTTP code: %d", i8_ret);
        faas_log_json_error_detail((const char*)t_http_config.t_http_response.ui8_response_data_buffer);
        ecf_set_has_connection(pt_ecf_cli, false);
        return 0;
    }
    else
    {
        COGNIT_LOG_TRACE("JSON received size: %ld", t_http_config.t_http_response.size);
        ecf_set_has_connection(pt_ecf_cli, true);

        i8_ret = faasparser_parse_json_str_as_exec_response((const char*) t_http_config.t_http_response.ui8_response_data_buffer, pt_exec_response);

        if (i8_ret != 0)
        {
            COGNIT_LOG_ERROR("Error parsing JSON");
        }
        else
        {
            COGNIT_LOG_DEBUG("Response JSON parsed correctly");
        }
    }

    // TODO IMPOORTANT handle the free of the response buffer???

    return i8_ret;
}

bool ecf_get_has_connection(edge_cluster_frontend_cli_t* pt_ecf_cli)
{
    return pt_ecf_cli->has_connection;
}

void ecf_set_has_connection(edge_cluster_frontend_cli_t* pt_ecf_cli, bool value)
{
    pt_ecf_cli->has_connection = value;
}
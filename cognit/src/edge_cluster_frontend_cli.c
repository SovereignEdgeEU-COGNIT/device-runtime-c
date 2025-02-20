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
}

int ecf_cli_upload_function_to_daas(edge_cluster_frontend_cli_t* pt_ecf_cli, char* biscuit_token, faas_t* pt_faas)
{
    int8_t i8_ret               = 0;
    http_config_t t_http_config = { 0 };
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
    snprintf(url, MAX_URL_LENGTH, "%s/%s", pt_ecf_cli->t_ecf_endpoint, FUNC_UPLOAD_ENDPOINT);

    t_http_config.c_url           = url;
    t_http_config.c_method        = HTTP_METHOD_POST;
    t_http_config.ui32_timeout_ms = ECF_REQ_TIMEOUT * 1000;
    t_http_config.c_token         = biscuit_token;
    t_http_config.binary          = true;

    uint8_t ui8_payload[2048];
    int payload_len = faas_serialize_fc(pt_faas, ui8_payload, sizeof(ui8_payload));

    COGNIT_LOG_DEBUG("FaaS execute sync [POST-URL]: %s", t_http_config.c_url);
    i8_ret = cognit_http_send(ui8_payload, payload_len, &t_http_config);

    if (i8_ret != 0
        || t_http_config.t_http_response.ui8_response_data_buffer == NULL
        || t_http_config.t_http_response.size == 0
        || t_http_config.t_http_response.l_http_code != 200)
    {
        COGNIT_LOG_ERROR("Error sending HTTP request, HTTP code: %d", i8_ret);
        ecf_set_has_connection(pt_ecf_cli, false);
        return 0;
    }
    else
    {
        COGNIT_LOG_TRACE("JSON received size: %ld", t_http_config.t_http_response.size);
        ecf_set_has_connection(pt_ecf_cli, false);

        // Copy the response json to the response struct
        i8_ret = faas_deserialize_fc_upload_response(t_http_config.t_http_response.ui8_response_data_buffer, t_http_config.t_http_response.size);

        if (i8_ret == 0)
        {
            COGNIT_LOG_ERROR("Error parsing JSON");
        }
    }

    // TODO IMPOORTANT handle the free of the response buffer???

    return i8_ret;
}

int ecf_cli_faas_exec_sync(edge_cluster_frontend_cli_t* pt_ecf_cli, char* biscuit_token, faas_t* pt_faas, void** pt_exec_response)
{
    int8_t i8_ret               = 0;
    http_config_t t_http_config = { 0 };
    uint8_t ui8_payload[2048];
    char url[MAX_URL_LENGTH];

    if (pt_ecf_cli == NULL)
    {
        COGNIT_LOG_ERROR("Cognit frontend not initialized");
        set_has_connection(pt_ecf_cli, false);
        return 0;
    }

    if (biscuit_token == NULL)
    {
        COGNIT_LOG_ERROR("Token not provided");
        ecf_set_has_connection(pt_ecf_cli, false);
        return 0;
    }

    memset(url, 0, sizeof(url));
    snprintf(url, MAX_URL_LENGTH, "%s/%s", pt_ecf_cli->t_ecf_endpoint, FAAS_REQUEST_ENDPOINT);

    t_http_config.c_url           = url;
    t_http_config.c_method        = HTTP_METHOD_POST;
    t_http_config.ui32_timeout_ms = ECF_REQ_TIMEOUT * 1000;
    t_http_config.c_token         = biscuit_token;
    t_http_config.binary          = true;

    int payload_len = faas_serialize_faas_request(pt_faas, ui8_payload, sizeof(ui8_payload));

    if (payload_len == 0)
    {
        return -1;
    }

    i8_ret = cognit_http_send(ui8_payload, payload_len, &t_http_config);
    COGNIT_LOG_DEBUG("FaaS execute sync [POST-URL]: %s", t_http_config.c_url);

    if (i8_ret != 0
        || t_http_config.t_http_response.ui8_response_data_buffer == NULL
        || t_http_config.t_http_response.size == 0
        || t_http_config.t_http_response.l_http_code != 200)
    {
        COGNIT_LOG_ERROR("Error sending HTTP request, HTTP code: %d", i8_ret);
        ecf_set_has_connection(pt_ecf_cli, false);
        return 0;
    }
    else
    {
        COGNIT_LOG_TRACE("JSON received size: %ld", t_http_config.t_http_response.size);
        ecf_set_has_connection(pt_ecf_cli, true);

        // Copy the response json to the response struct
        i8_ret = faas_deserialize_faas_response(t_http_config.t_http_response.ui8_response_data_buffer, t_http_config.t_http_response.size, pt_exec_response);

        if (i8_ret != 0)
        {
            COGNIT_LOG_ERROR("Error parsing JSON");
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
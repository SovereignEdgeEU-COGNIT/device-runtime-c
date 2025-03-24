#include <cognit_frontend_cli.h>
#include <cognit_http.h>
#include <stdlib.h>
#include <string.h>
#include <cf_parser.h>
#include <logger.h>

int cognit_frontend_cli_init(cognit_frontend_cli_t* pt_cognit_frontend_cli, cognit_config_t* pt_cognit_config)
{
    if (pt_cognit_frontend_cli == NULL)
    {
        COGNIT_LOG_ERROR("Cognit frontend not initialized");
        return -1;
    }

    if (pt_cognit_config == NULL)
    {
        COGNIT_LOG_ERROR("Cognit config not provided");
        return -1;
    }

    memset(pt_cognit_frontend_cli, 0, sizeof(cognit_frontend_cli_t));

    cfc_set_has_connection(pt_cognit_frontend_cli, false);

    // Setup the instance to point to the configuration
    pt_cognit_frontend_cli->m_t_config = pt_cognit_config;

    return 0;
}

int cognit_frontend_cli_authenticate(cognit_frontend_cli_t* pt_cognit_frontend_cli, char* token)
{
    int8_t i8_ret = 0;
    uint8_t ui8_payload[1024 * 16];
    size_t payload_len          = 0;
    http_config_t t_http_config = { 0 };
    char url[MAX_URL_LENGTH];

    if (pt_cognit_frontend_cli == NULL)
    {
        COGNIT_LOG_ERROR("Cognit frontend not initialized");
        return -1;
    }

    memset(url, 0, sizeof(url));
    snprintf(url, MAX_URL_LENGTH, "%s/%s", pt_cognit_frontend_cli->m_t_config->cognit_frontend_endpoint, CF_AUTH_ENDPOINT);

    t_http_config.c_url           = url;
    t_http_config.c_method        = HTTP_METHOD_POST;
    t_http_config.ui32_timeout_ms = CFC_REQ_TIMEOUT * 1000;
    t_http_config.c_username      = pt_cognit_frontend_cli->m_t_config->cognit_frontend_usr;
    t_http_config.c_password      = pt_cognit_frontend_cli->m_t_config->cognit_frontend_pwd;

    COGNIT_LOG_DEBUG("Requiesting token to %s", url);
    i8_ret = cognit_http_send(ui8_payload, payload_len, &t_http_config);

    if (i8_ret != 0
        || (t_http_config.t_http_response.l_http_code != 200
            && t_http_config.t_http_response.l_http_code != 201))
    {
        COGNIT_LOG_ERROR("Token creation failed with status code: %ld", t_http_config.t_http_response.l_http_code);
        COGNIT_LOG_ERROR("i8_ret: %d", i8_ret);
        faas_log_json_error_detail((const char*)t_http_config.t_http_response.ui8_response_data_buffer);
        cfc_set_has_connection(pt_cognit_frontend_cli, false);

        return -1;
    }
    else
    {
        COGNIT_LOG_DEBUG("Response JSON: %s", t_http_config.t_http_response.ui8_response_data_buffer);
        // Copy the response json to the token string
        cfparser_parse_str_response_as_token(token, t_http_config.t_http_response.ui8_response_data_buffer);

        if (token[0] == '\0')
        {
            COGNIT_LOG_ERROR("Error parsing token");
            cfc_set_has_connection(pt_cognit_frontend_cli, false);
            return -1;
        }
    }

    cfc_set_has_connection(pt_cognit_frontend_cli, true);
    // TODO IMPOORTANT handle the free of the response buffer???

    return 0;
}

int cognit_frontend_cli_update_requirements(cognit_frontend_cli_t* pt_cognit_frontend_cli, char* biscuit_token, scheduling_t t_app_reqs, int* pt_app_req_id)
{
    int8_t i8_ret = 0;
    uint8_t ui8_payload[1024 * 16];
    size_t payload_len;
    http_config_t t_http_config = { 0 };
    char url[MAX_URL_LENGTH];

    if (pt_cognit_frontend_cli == NULL)
    {
        COGNIT_LOG_ERROR("Cognit frontend not initialized");
        cfc_set_has_connection(pt_cognit_frontend_cli, false);
        return -1;
    }

    if (biscuit_token == NULL)
    {
        COGNIT_LOG_ERROR("Token not provided");
        return -1;
    }

    memset(url, 0, sizeof(url));
    snprintf(url, MAX_URL_LENGTH, "%s/%s", pt_cognit_frontend_cli->m_t_config->cognit_frontend_endpoint, CF_REQ_ENDPOINT);

    t_http_config.c_url           = url;
    t_http_config.c_method        = HTTP_METHOD_POST;
    t_http_config.ui32_timeout_ms = CFC_REQ_TIMEOUT * 1000;
    t_http_config.c_token         = biscuit_token;

    i8_ret = cfparser_parse_requirements_as_str_json(&t_app_reqs, ui8_payload, &payload_len);
    COGNIT_LOG_DEBUG("Requirements JSON: %s", ui8_payload);

    COGNIT_LOG_DEBUG("Sending requirements to %s", url);
    i8_ret = cognit_http_send(ui8_payload, payload_len, &t_http_config);

    if (i8_ret != 0
        || (t_http_config.t_http_response.l_http_code != 200
            && t_http_config.t_http_response.l_http_code != 201))
    {
        COGNIT_LOG_ERROR("Requirements update failed with status code: %ld", t_http_config.t_http_response.l_http_code);
        COGNIT_LOG_ERROR("i8_ret: %d", i8_ret);
        faas_log_json_error_detail((const char*)t_http_config.t_http_response.ui8_response_data_buffer);
        cfc_set_has_connection(pt_cognit_frontend_cli, false);

        return -1;
    }
    else
    {
        cfc_set_has_connection(pt_cognit_frontend_cli, true);
        COGNIT_LOG_DEBUG("Response JSON: %s", t_http_config.t_http_response.ui8_response_data_buffer);
        // Copy the response json to the token string
        *pt_app_req_id = atoi((char*)t_http_config.t_http_response.ui8_response_data_buffer);
        if (i8_ret != 0)
        {
            COGNIT_LOG_ERROR("Error parsing token");
            return -1;
        }
    }

    // TODO IMPOORTANT handle the free of the response buffer???

    return 0;
}

int cognit_frontend_cli_get_ecf_address(cognit_frontend_cli_t* pt_cognit_frontend_cli, char* biscuit_token, int app_req_id)
{
    int8_t i8_ret = 0;
    uint8_t ui8_payload[1024 * 16];
    size_t payload_len;
    http_config_t t_http_config = { 0 };
    char url[MAX_URL_LENGTH];

    if (pt_cognit_frontend_cli == NULL)
    {
        COGNIT_LOG_ERROR("Cognit frontend not initialized");
        cfc_set_has_connection(pt_cognit_frontend_cli, false);
        return -1;
    }

    if (biscuit_token == NULL)
    {
        COGNIT_LOG_ERROR("Token not provided");
        cfc_set_has_connection(pt_cognit_frontend_cli, false);
        return -1;
    }

    memset(url, 0, sizeof(url));
    snprintf(url, MAX_URL_LENGTH, "%s/%s/%d/%s", pt_cognit_frontend_cli->m_t_config->cognit_frontend_endpoint, CF_REQ_ENDPOINT, app_req_id, CF_ECF_ADDRESS_ENDPOINT);

    t_http_config.c_url           = url;
    t_http_config.c_method        = HTTP_METHOD_GET;
    t_http_config.ui32_timeout_ms = CFC_REQ_TIMEOUT * 1000;
    t_http_config.c_token         = biscuit_token;

    COGNIT_LOG_DEBUG("Sending ECF address request to %s", url);
    i8_ret = cognit_http_send(ui8_payload, payload_len, &t_http_config);

    if (i8_ret != 0
        || (t_http_config.t_http_response.l_http_code != 200
            && t_http_config.t_http_response.l_http_code != 201))
    {
        COGNIT_LOG_ERROR("Get ECF failed with status code: %ld", t_http_config.t_http_response.l_http_code);
        COGNIT_LOG_ERROR("i8_ret: %d", i8_ret);
        faas_log_json_error_detail((const char*)t_http_config.t_http_response.ui8_response_data_buffer);
        cfc_set_has_connection(pt_cognit_frontend_cli, false);

        return -1;
    }
    else
    {
        COGNIT_LOG_DEBUG("Response JSON: %s", t_http_config.t_http_response.ui8_response_data_buffer);
        // Copy the response json to the token string
        cfparser_parse_json_str_as_ecf_address(t_http_config.t_http_response.ui8_response_data_buffer, &pt_cognit_frontend_cli->ecf_resp);
        COGNIT_LOG_DEBUG("ECF IP: %s", pt_cognit_frontend_cli->ecf_resp.template);
        if (i8_ret != 0)
        {
            COGNIT_LOG_ERROR("Error parsing token");
            cfc_set_has_connection(pt_cognit_frontend_cli, false);
            return -1;
        }
    }

    cfc_set_has_connection(pt_cognit_frontend_cli, true);

    // TODO IMPOORTANT handle the free of the response buffer???

    return 0;
}

int cfc_cli_upload_function_to_daas(cognit_frontend_cli_t* pt_cfc_cli, char* biscuit_token, faas_t* pt_faas)
{
    int8_t i8_ret               = 0;
    uint8_t ui8_payload[1024 * 16];
    size_t payload_len          = 0;
    http_config_t t_http_config = { 0 };
    char url[MAX_URL_LENGTH];
    unsigned int fc_id             = 0;

    if (pt_cfc_cli == NULL)
    {
        COGNIT_LOG_ERROR("Cognit frontend not initialized");
        cfc_set_has_connection(pt_cfc_cli, false);
        return 0;
    }

    if (biscuit_token == NULL)
    {
        COGNIT_LOG_ERROR("Token not provided");
        cfc_set_has_connection(pt_cfc_cli, false);
        return 0;
    }

    memset(url, 0, sizeof(url));
    snprintf(url, MAX_URL_LENGTH, "%s/%s", pt_cfc_cli->m_t_config->cognit_frontend_endpoint, FUNC_UPLOAD_ENDPOINT);

    t_http_config.c_url           = url;
    t_http_config.c_method        = HTTP_METHOD_POST;
    t_http_config.ui32_timeout_ms = CFC_REQ_TIMEOUT * 1000;
    t_http_config.c_token         = biscuit_token;
    t_http_config.binary          = false;

    if (faasparser_parse_fc_as_str_json(pt_faas, ui8_payload, &payload_len) == JSON_ERR_CODE_OK)
    {
        COGNIT_LOG_DEBUG("FC parsed successfully, generated JSON: %s", ui8_payload);
    }

    COGNIT_LOG_DEBUG("FaaS execute sync [POST-URL]: %s", t_http_config.c_url);
    i8_ret = cognit_http_send(ui8_payload, payload_len, &t_http_config);

    if (i8_ret != 0
        || t_http_config.t_http_response.ui8_response_data_buffer == NULL
        || t_http_config.t_http_response.size == 0
        || t_http_config.t_http_response.l_http_code != 200)
    {
        COGNIT_LOG_ERROR("Error sending HTTP request, HTTP code: %d", i8_ret);
        faas_log_json_error_detail((const char*)t_http_config.t_http_response.ui8_response_data_buffer);
        cfc_set_has_connection(pt_cfc_cli, false);
        return 0;
    }
    else
    {
        COGNIT_LOG_TRACE("JSON received size: %ld", t_http_config.t_http_response.size);
        cfc_set_has_connection(pt_cfc_cli, false);

        // Copy the response json to the response struct
        fc_id = atoi((char*)t_http_config.t_http_response.ui8_response_data_buffer);
        if (fc_id == 0)
        {
            COGNIT_LOG_ERROR("Error parsing JSON");
        }
    }

    // TODO IMPOORTANT handle the free of the response buffer???

    return fc_id;
}



void cfc_set_has_connection(cognit_frontend_cli_t* pt_cognit_frontend_cli, bool value)
{
    pt_cognit_frontend_cli->has_connection = value;
}

bool cfc_get_has_connection(cognit_frontend_cli_t* pt_cognit_frontend_cli)
{
    return pt_cognit_frontend_cli->has_connection;
}
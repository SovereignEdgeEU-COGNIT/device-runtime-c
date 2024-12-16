#include <cognit_frontend_cli.h>
#include <cognit_http.h>
#include <stdlib.h>
#include <string.h>
#include <cf_parser.h>
#include <logger.h>

int cognit_frontend_cli_init(cognit_frontend_cli_t* pt_cognit_frontend_cli, cognit_config_t* pt_cognit_config)
{
    if(pt_cognit_frontend_cli == NULL)
    {
        COGNIT_LOG_ERROR("Cognit frontend not initialized");
        return -1;
    }

    if(pt_cognit_config == NULL)
    {
        COGNIT_LOG_ERROR("Cognit config not provided");
        return -1;
    }

    // Setup the instance to point to the configuration
    pt_cognit_frontend_cli->m_t_config = pt_cognit_config;

    return 0;
}

int cognit_frontend_cli_authenticate(cognit_frontend_cli_t* pt_cognit_frontend_cli, char* token)
{
    int8_t i8_ret = 0;
    uint8_t ui8_payload[1024 * 16];
    size_t payload_len;
    http_config_t t_http_config = { 0 };
    char url[MAX_URL_LENGTH];

    if(pt_cognit_frontend_cli == NULL)
    {
        COGNIT_LOG_ERROR("Cognit frontend not initialized");
        return -1;
    }

    memset(url, 0, sizeof(url));
    snprintf(url, MAX_URL_LENGTH, "%s://%s/%s", STR_PROTOCOL, pt_cognit_frontend_cli->m_t_config->cognit_frontend_endpoint, CF_AUTH_ENDPOINT);

    t_http_config.c_url           = url;
    t_http_config.c_method        = HTTP_METHOD_POST;
    t_http_config.ui32_timeout_ms = REQ_TIMEOUT * 1000;
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

        return COGNIT_ECODE_ERROR;
    }
    else
    {
        COGNIT_LOG_DEBUG("Response JSON: %s", t_http_config.t_http_response.ui8_response_data_buffer);
        // Copy the response json to the token string
        cfparser_parse_str_response_as_token(token, t_http_config.t_http_response.ui8_response_data_buffer);

        if (token[0] == '\0')
        {
            COGNIT_LOG_ERROR("Error parsing token");
            return COGNIT_ECODE_ERROR;
        }
    }

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

    if(pt_cognit_frontend_cli == NULL)
    {
        COGNIT_LOG_ERROR("Cognit frontend not initialized");
        return -1;
    }

    if(biscuit_token == NULL)
    {
        COGNIT_LOG_ERROR("Token not provided");
        return -1;
    }

    memset(url, 0, sizeof(url));
    snprintf(url, MAX_URL_LENGTH, "%s://%s/%s", STR_PROTOCOL, pt_cognit_frontend_cli->m_t_config->cognit_frontend_endpoint, CF_REQ_ENDPOINT);

    t_http_config.c_url           = url;
    t_http_config.c_method        = HTTP_METHOD_POST;
    t_http_config.ui32_timeout_ms = REQ_TIMEOUT * 1000;
    t_http_config.c_token         = biscuit_token;

    i8_ret = cfparser_parse_requirements_as_str_json(&t_app_reqs, &ui8_payload, &payload_len);
    COGNIT_LOG_DEBUG("Requirements JSON: %s", ui8_payload);

    COGNIT_LOG_DEBUG("Sending requirements to %s", url);
    i8_ret = cognit_http_send(ui8_payload, payload_len, &t_http_config);

    if (i8_ret != 0
        || (t_http_config.t_http_response.l_http_code != 200
        && t_http_config.t_http_response.l_http_code != 201))
    {
        COGNIT_LOG_ERROR("Requirements update failed with status code: %s", t_http_config.t_http_response.l_http_code);
        COGNIT_LOG_ERROR("i8_ret: %d", i8_ret);

        return COGNIT_ECODE_ERROR;
    }
    else
    {
        COGNIT_LOG_DEBUG("Response JSON: %s", t_http_config.t_http_response.ui8_response_data_buffer);
        // Copy the response json to the token string
        *pt_app_req_id = atoi((char*)t_http_config.t_http_response.ui8_response_data_buffer);
        printf("apr_req_id: %d", *pt_app_req_id);
        if (i8_ret != 0)
        {
            COGNIT_LOG_ERROR("Error parsing token");
            return COGNIT_ECODE_ERROR;
        }
    }

    // TODO IMPOORTANT handle the free of the response buffer???

    return 0;
}

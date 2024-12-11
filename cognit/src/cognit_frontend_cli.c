#include <cognit_frontend_cli.h>
#include <cognit_http.h>
#include <stdlib.h>
#include <string.h>
#include <cf_parser.h>
#include <logger.h>

int cognit_frontend_cli_init(cognit_frontend_cli_t* pt_cognit_frontend_cli, cognit_config_t* pt_cognit_config)
{
    // Setup the instance to point to the configuration
    pt_cognit_frontend_cli->m_t_config = pt_cognit_config;

    return 0;
}

int cognit_frontend_cli_authenticate(cognit_frontend_cli_t* pt_cognit_frontend_cli, char token[])
{
    int8_t i8_ret = 0;
    uint8_t ui8_payload[1024 * 16];
    size_t payload_len;
    http_config_t t_http_config = { 0 };
    char url[MAX_URL_LENGTH];

    memset(url, 0, sizeof(url));
    snprintf(url, MAX_URL_LENGTH, "%s://%s", STR_PROTOCOL, pt_cognit_frontend_cli->m_t_config->cognit_frontend_endpoint, CF_AUTH_ENDPOINT);

    t_http_config.c_url           = url;
    t_http_config.c_method        = HTTP_METHOD_POST;
    t_http_config.ui32_timeout_ms = REQ_TIMEOUT * 1000;
    t_http_config.c_username      = pt_cognit_frontend_cli->m_t_config->cognit_frontend_usr;
    t_http_config.c_password      = pt_cognit_frontend_cli->m_t_config->cognit_frontend_pwd;

    COGNIT_LOG_DEBUG("Requiesting token for %s", pt_cognit_frontend_cli->m_t_config->cognit_frontend_usr);
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
        strcpy((char*)t_http_config.t_http_response.ui8_response_data_buffer, pt_cognit_frontend_cli->biscuit_token);

        if (i8_ret != 0)
        {
            COGNIT_LOG_ERROR("Error parsing token");
            return COGNIT_ECODE_ERROR;
        }
    }

    // TODO IMPOORTANT handle the free of the response buffer???

    return 0;
}

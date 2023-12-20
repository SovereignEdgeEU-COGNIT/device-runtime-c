#include "cognit_http.h"
#include <stdio.h>

extern int my_http_send_req_cb(const char* c_buffer, size_t size, http_config_t* config);
send_http_req_cb_t http_send_req_cb = my_http_send_req_cb;

void cognit_http_init(http_config_t* config)
{
    config->t_http_response.ui8_response_data_buffer = NULL;
    config->c_url                                    = NULL;
    config->c_method                                 = NULL;
    config->ui32_timeout_ms                          = 3000;
}

int cognit_http_send(const char* c_buffer, size_t size, http_config_t* config)
{
    return http_send_req_cb(c_buffer, size, config);
}
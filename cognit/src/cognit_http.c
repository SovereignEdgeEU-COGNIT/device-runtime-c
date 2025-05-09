#include "cognit_http.h"
#include <stdio.h>
extern int8_t my_http_send_req_cb(const char* c_buffer, size_t size, http_config_t* config);

int8_t cognit_http_send(const char* c_buffer, size_t size, http_config_t* config)
{
    return my_http_send_req_cb(c_buffer, size, config);
}
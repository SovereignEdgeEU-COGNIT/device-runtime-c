
#include <stdio.h>
#include "cognit_http.h"
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

size_t handle_response_data_cb(void* data_content, size_t size, size_t nmemb, void* user_buffer)
{
    size_t realsize           = size * nmemb;
    http_response_t* response = (http_response_t*)user_buffer;

    uint8_t* ui8_buffer_ptr = (uint8_t*)realloc(response->ui8_response_data_buffer, response->size + realsize + 1);

    if (ui8_buffer_ptr == NULL)
    {
        fprintf(stderr, "[handle_response_data] Realloc() failed\n");
        return 0;
    }

    response->ui8_response_data_buffer = ui8_buffer_ptr;
    memcpy(&(response->ui8_response_data_buffer[response->size]), data_content, realsize);
    response->size += realsize;
    response->ui8_response_data_buffer[response->size] = '\0';

    // TODO: free(ui8_response_data_buffer) after reading content

    return realsize;
}

int8_t my_http_send_req(const char* c_buffer, size_t size, http_config_t* config)
{
    CURL* curl;
    CURLcode res;
    long http_code                                   = 0;
    struct curl_slist* headers                       = NULL;
    config->t_http_response.ui8_response_data_buffer = (uint8_t*)malloc(1);
    config->t_http_response.size                     = 0;

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
            fprintf(stderr, "[hhtp_send_req_cb] curl_easy_setopt() failed\n");
            return -1;
        }

        if (config->c_method == HTTP_METHOD_GET)
        {
            if (curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L) != CURLE_OK)
            {
                fprintf(stderr, "[hhtp_send_req_cb] curl_easy_setopt()->get() failed\n");
                return -1;
            }
        }
        else if (config->c_method == HTTP_METHOD_POST)
        {
            if (curl_easy_setopt(curl, CURLOPT_POST, 1L) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST") != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, size) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_POSTFIELDS, c_buffer) != CURLE_OK)
            {
                fprintf(stderr, "[hhtp_send_req_cb] curl_easy_setopt()->post() failed\n");
                return -1;
            }
        }
        else
        {
            fprintf(stderr, "[hhtp_send_req_cb] Invalid HTTP method\n");
            return -1;
        }

        // Make the request
        res = curl_easy_perform(curl);

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        fprintf(stderr, "HTTP err code %ld \n", http_code);

        // Check errors
        if (res != CURLE_OK)
        {
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            fprintf(stderr, "HTTP err code %ld \n", http_code);
        }

        // Clean and close CURL session
        curl_easy_cleanup(curl);
    }

    // Clean global curl configuration
    curl_global_cleanup();
    // User must free(t_http_response.ui8_response_data_buffer) after reading content!!
    return (res == CURLE_OK) ? 0 : -1;
}

int main()
{

    // Initialize HTTP client
    // cognit_http_send

    return 0;
}

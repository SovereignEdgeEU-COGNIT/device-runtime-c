#include <utest_http.h>
#include <curl/curl.h>
extern "C" {
#include <cognit_http.h>
#include <logger.h>
#include <ip_utils.h>
}

size_t handle_response_data_cb(void* data_content, size_t size, size_t nmemb, void* user_buffer)
{
    size_t realsize           = size * nmemb;
    http_response_t* response = (http_response_t*)user_buffer;

    if (response->size + realsize >= sizeof(response->ui8_response_data_buffer))
    {
        COGNIT_LOG_ERROR("Response buffer too small");
        return 0;
    }

    memcpy(&(response->ui8_response_data_buffer[response->size]), data_content, realsize);
    response->size += realsize;
    response->ui8_response_data_buffer[response->size] = '\0';

    return realsize;
}

extern "C" {
int my_http_send_req_cb(const char* c_buffer, size_t size, http_config_t* config)
{
    CURL* curl;
    CURLcode res;
    long http_code             = 0;
    struct curl_slist* headers = NULL;
    memset(&config->t_http_response.ui8_response_data_buffer, 0, sizeof(config->t_http_response.ui8_response_data_buffer));
    config->t_http_response.size = 0;

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
            COGNIT_LOG_ERROR("[hhtp_send_req_cb] curl_easy_setopt() failed");
            return -1;
        }

        // Find '[' or ']' in the URL to determine the IP version
        // TODO: fix ip_utils to obtain http://[2001:67c:22b8:1::d]:8000/v1/faas/execute-sync
        // as IP_V6
        if (strchr(config->c_url, '[') != NULL
            && strchr(config->c_url, ']') != NULL)
        {
            if (curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V6) != CURLE_OK)
            {
                COGNIT_LOG_ERROR("[hhtp_send_req_cb] curl_easy_setopt()->IPRESOLVE_V6 failed");
                return -1;
            }
        }

        if (strcmp(config->c_method, HTTP_METHOD_GET) == 0)
        {
            if (curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_USERNAME, config->c_username) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_PASSWORD, config->c_password) != CURLE_OK)
            {
                COGNIT_LOG_ERROR("[hhtp_send_req_cb] curl_easy_setopt()->get() failed");
                return -1;
            }
        }
        else if (strcmp(config->c_method, HTTP_METHOD_POST) == 0)
        {
            if (curl_easy_setopt(curl, CURLOPT_POST, 1L) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST") != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, size) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_POSTFIELDS, c_buffer) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_USERNAME, config->c_username) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_PASSWORD, config->c_password) != CURLE_OK)
            {
                COGNIT_LOG_ERROR("[hhtp_send_req_cb] curl_easy_setopt()->post() failed");
                return -1;
            }
        }
        else if (strcmp(config->c_method, HTTP_METHOD_DELETE) == 0)
        {
            if (curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE") != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_USERNAME, config->c_username) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_PASSWORD, config->c_password) != CURLE_OK)
            {
                COGNIT_LOG_ERROR("[hhtp_send_req_cb] curl_easy_setopt()->post() failed");
                return -1;
            }
        }
        else
        {
            COGNIT_LOG_ERROR("[hhtp_send_req_cb] Invalid HTTP method");
            return -1;
        }

        // Make the request
        res = curl_easy_perform(curl);

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        COGNIT_LOG_ERROR("HTTP err code %ld ", http_code);

        // Check errors
        if (res != CURLE_OK)
        {
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            COGNIT_LOG_ERROR("curl_easy_perform() failed: %s", curl_easy_strerror(res));
            COGNIT_LOG_ERROR("HTTP err code %ld ", http_code);
        }

        // Clean and close CURL session
        curl_easy_cleanup(curl);
    }

    config->t_http_response.l_http_code = http_code;

    // Clean global curl configuration
    curl_global_cleanup();
    free(headers);

    return (res == CURLE_OK) ? 0 : -1;
}
}

TEST_F(UTestHttp, TestHttpGet)
{
    int8_t i8_ret                             = 0;
    char c_buffer[MAX_HTTP_TRANSMISSION_SIZE] = { 0 };
    size_t size                               = 0;
    http_config_t config                      = { 0 };
    config.c_method                           = HTTP_METHOD_GET;
    config.c_url                              = "https://jsonplaceholder.typicode.com/posts";
    config.ui32_timeout_ms                    = 5000;
    config.c_username                         = "user";
    config.c_password                         = "password";

    i8_ret = cognit_http_send(c_buffer, size, &config);

    // Print json response
    COGNIT_LOG_DEBUG("%s", config.t_http_response.ui8_response_data_buffer);
    COGNIT_LOG_DEBUG("size: %ld", config.t_http_response.size);

    ASSERT_EQ(i8_ret, CURLE_OK);
}

TEST_F(UTestHttp, TestHttpPost)
{
    int8_t i8_ret                             = 0;
    const char* c_json_response               = "{\"title\":\"me\",\"body\":\"myproject\",\"userId\":9,\"id\":101}";
    const char* c_json_test                   = "{\"title\":\"me\",\"body\":\"myproject\",\"userId\":9}";
    char c_buffer[MAX_HTTP_TRANSMISSION_SIZE] = { 0 };
    size_t size                               = 0;
    http_config_t config;
    config.c_method        = HTTP_METHOD_POST;
    config.c_url           = "https://jsonplaceholder.typicode.com/posts";
    config.ui32_timeout_ms = 5000;

    strncpy(c_buffer, c_json_test, strlen(c_json_test) + 1);
    size = strlen(c_json_test);

    i8_ret = cognit_http_send(c_buffer, size, &config);

    // Print json response
    COGNIT_LOG_DEBUG("%s", config.t_http_response.ui8_response_data_buffer);
    COGNIT_LOG_DEBUG("size: %ld", config.t_http_response.size);

    // TODO: make proper json comparison -> Problem with , '\0' and spaces but same content
    EXPECT_EQ(i8_ret, CURLE_OK);
}

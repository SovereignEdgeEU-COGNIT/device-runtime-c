#include <prov_engine_cli.h>
#include <string.h>

int8_t prov_eng_init(prov_eng_cli_context_t* t_pec_context)
{
    int8_t i8_ret = PE_ERR_CODE_SUCCESS;

    m_t_pec_context = &t_pec_context;
    // TODO: validate config

    return i8_ret;
}

serverless_runtime_conf_t prov_eng_create_runtime(serverless_runtime_conf_t* serverless_runtime_context)
{
    serverless_runtime_conf_t t_ret;
    return t_ret;
}
serverless_runtime_conf_t prov_eng_retreive_runtime(uint32_t ui32_id)
{
    serverless_runtime_conf_t t_ret;
    return t_ret;
}
bool prov_eng_delete_runtime(uint32_t ui32_id)
{
    bool b_ret = false;
    return b_ret;
}
// client->endpoint = malloc(256 * sizeof(char));
// snprintf(client->endpoint, 256, "http://%s:%d", config.prov_engine_endpoint, config.prov_engine_port);
// return client;

// ProvEngineClient* prov_engine_client_create(CognitConfig config) {
//     ProvEngineClient* client = malloc(sizeof(ProvEngineClient));
//     client->config = config;
// }

// ServerlessRuntime* prov_engine_client_create_runtime(ProvEngineClient* client, ServerlessRuntime* serverless_runtime) {
//     ServerlessRuntime* response = NULL;

//     char url[256];
//     snprintf(url, 256, "%s/%s", client->endpoint, SR_RESOURCE_ENDPOINT);

//     char* aux_dict = filter_empty_values(serverless_runtime);
//     printf("Create [POST] URL: %s\n", url);

//     CURL* curl = curl_easy_init();
//     if (curl) {
//         struct curl_slist* headers = NULL;
//         headers = curl_slist_append(headers, "Content-Type: application/json");

//         curl_easy_setopt(curl, CURLOPT_URL, url);
//         curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
//         curl_easy_setopt(curl, CURLOPT_POSTFIELDS, aux_dict);
//         curl_easy_setopt(curl, CURLOPT_TIMEOUT, REQ_TIMEOUT);

//         CURLcode res = curl_easy_perform(curl);
//         if (res != CURLE_OK) {
//             fprintf(stderr, "Provisioning engine request failed: %s\n", curl_easy_strerror(res));
//         } else {
//             long http_code = 0;
//             curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
//             if (http_code != 201) {
//                 fprintf(stderr, "Provisioning engine returned %ld on create\n", http_code);
//             } else {
//                 json_error_t error;
//                 json_t* json_response = json_loads(aux_dict, 0, &error);
//                 if (!json_response) {
//                     fprintf(stderr, "Failed to parse JSON response: %s\n", error.text);
//                 } else {
//                     response = malloc(sizeof(ServerlessRuntime));
//                     response->name = strdup(json_string_value(json_object_get(json_response, "name")));
//                     // Parse other fields as needed
//                     json_decref(json_response);
//                 }
//             }
//         }

//         curl_slist_free_all(headers);
//         curl_easy_cleanup(curl);
//     }

//     free(aux_dict);
//     return response;
// }

// ServerlessRuntime* prov_engine_client_retrieve_runtime(ProvEngineClient* client, int sr_id) {
//     ServerlessRuntime* response = NULL;

//     char url[256];
//     snprintf(url, 256, "%s/%s/%d", client->endpoint, SR_RESOURCE_ENDPOINT, sr_id);
//     printf("Retrieve [GET] URL: %s\n", url);

//     CURL* curl = curl_easy_init();
//     if (curl) {
//         curl_easy_setopt(curl, CURLOPT_URL, url);
//         curl_easy_setopt(curl, CURLOPT_TIMEOUT, REQ_TIMEOUT);

//         CURLcode res = curl_easy_perform(curl);
//         if (res != CURLE_OK) {
//             fprintf(stderr, "Provisioning engine request failed: %s\n", curl_easy_strerror(res));
//         } else {
//             long http_code = 0;
//             curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
//             if (http_code != 200) {
//                 fprintf(stderr, "Provisioning engine returned %ld on retrieve\n", http_code);
//             } else {
//                 char* response_str = NULL;
//                 size_t response_len = 0;
//                 curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &response_len);
//                 response_str = malloc((response_len + 1) * sizeof(char));
//                 if (response_str) {
//                     response_str[response_len] = '\0';
//                     curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
//                     if (http_code == 200) {
//                         strncpy(response_str, response_len);
//                         json_error_t error;
//                         json_t* json_response = json_loads(response_str, 0, &error);
//                         if (!json_response) {
//                             fprintf(stderr, "Failed to parse JSON response: %s\n", error.text);
//                         } else {
//                             response = malloc(sizeof(ServerlessRuntime));
//                             response->name = strdup(json_string_value(json_object_get(json_response, "name")));
//                             // Parse other fields as needed
//                             json_decref(json_response);
//                         }
//                     }
//                     free(response_str);
//                 }
//             }
//         }

//         curl_easy_cleanup(curl);
//     }

//     return response;
// }

// bool prov_engine_client_delete_runtime(ProvEngineClient* client, int sr_id) {
//     bool success = false;

//     char url[256];
//     snprintf(url, 256, "%s/%s/%d", client->endpoint, SR_RESOURCE_ENDPOINT, sr_id);
//     printf("Delete [DELETE] URL: %s\n", url);

//     CURL* curl = curl_easy_init();
//     if (curl) {
//         curl_easy_setopt(curl, CURLOPT_URL, url);
//         curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
//         curl_easy_setopt(curl, CURLOPT_TIMEOUT, REQ_TIMEOUT);

//         CURLcode res = curl_easy_perform(curl);
//         if (res != CURLE_OK) {
//             fprintf(stderr, "Provisioning engine request failed: %s\n", curl_easy_strerror(res));
//         } else {
//             long http_code = 0;
//             curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
//             if (http_code != 204) {
//                 fprintf(stderr, "Provisioning engine returned %ld on delete\n", http_code);
//             } else {
//                 success = true;
//             }
//         }

//         curl_easy_cleanup(curl);
//     }

//     return success;
// }

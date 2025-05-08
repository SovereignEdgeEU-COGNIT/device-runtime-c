#include <faas_parser.h>
#include <cJSON.h>
#include <logger.h>
#include "cognit_encoding.h"

int8_t faasparser_parse_exec_faas_params_as_str_json(faas_t* pt_faas, uint8_t* ui8_payload_buff, size_t* payload_len)
{
    cJSON* param          = NULL;
    cJSON* params_array   = NULL;
    char* str_faas_json   = NULL;
    char* str_param       = NULL;
    char* str_b64_param   = NULL;
    int i_coded_param_len = 0;
    int out_params_len        = 0;

    params_array = cJSON_CreateArray();
    if (params_array == NULL)
    {
        COGNIT_LOG_ERROR("Error creating cJSON array");
        cJSON_Delete(params_array);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    for (int i = 0; i < pt_faas->params_count; i++)
    {
        unsigned char str_b64_param[1024 * 16];
        size_t base64_len;
        param = cJSON_CreateObject();

        if (param == NULL)
        {
            COGNIT_LOG_ERROR("Error creating cJSON object");
            cJSON_Delete(params_array);
            return JSON_ERR_CODE_INVALID_JSON;
        }
        uint8_t str_param[1024 * 16];
        int param_len = pb_serialize_faas_param(pt_faas, i, str_param, sizeof(str_param));
        
        if (cognit_base64_encode(str_b64_param, sizeof(str_b64_param), &base64_len, str_param, param_len) != 0) {
            COGNIT_LOG_ERROR("Error coding in base64\n");
            return JSON_ERR_CODE_INVALID_JSON;
        }

        cJSON_AddItemToArray(params_array, cJSON_CreateString(str_b64_param));

        cJSON_Delete(param);
    }

    //cJSON_AddItemToObject(root, "PARAMS", params_array);
    str_faas_json = cJSON_Print(params_array);

    // Copy the json string to the payload buffer
    strcpy((char*)ui8_payload_buff, str_faas_json);
    *payload_len = strlen(str_faas_json);

    cJSON_Delete(params_array);
    free(str_faas_json);

    return JSON_ERR_CODE_OK;
}

int8_t faasparser_parse_json_str_as_exec_response(const char* json_str, void** pt_res)
{
    size_t out_len = 0;
    char res_payload[1024 * 16]; 
    exec_response_t t_exec_response;
    t_exec_response.res = pt_res;
    cJSON* root = cJSON_Parse(json_str);

    if (root == NULL)
    {
        COGNIT_LOG_ERROR("Error parsing JSON");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    cJSON* ret_code_item = cJSON_GetObjectItem(root, "ret_code");
    cJSON* res_item      = cJSON_GetObjectItem(root, "res");
    cJSON* err_item      = cJSON_GetObjectItem(root, "err");

    if (!cJSON_IsNumber(ret_code_item) || !cJSON_IsString(res_item))
    {
        COGNIT_LOG_ERROR("JSON content types are wrong");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    t_exec_response.ret_code = ret_code_item->valueint;

    // Decode base64 res_item
    int ret = cognit_base64_decode(res_payload, sizeof(res_payload), &out_len, (const unsigned char *) res_item->valuestring, strlen(res_item->valuestring));
    if (ret != 0) {
        COGNIT_LOG_ERROR("Error decoding base64: -0x%04x\n", -ret);
        return JSON_ERR_CODE_INVALID_JSON;
    }
    uint8_t i8_ret = pb_deserialize_faas_param(res_payload, out_len, t_exec_response.res);

    if (t_exec_response.res == NULL || i8_ret == -1)
    {
        COGNIT_LOG_ERROR("Error decoding base64");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    // TODO: parse err ??

    cJSON_Delete(root);

    return JSON_ERR_CODE_OK;
}

void faas_log_json_error_detail(const char* response_body)
{
    if (response_body == NULL || strlen(response_body) == 0)
        return;

    cJSON* root = cJSON_Parse(response_body);
    if (root == NULL)
    {
        const char* error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            COGNIT_LOG_ERROR("Error parsing JSON: %s", error_ptr);
        }
        return;
    }

    cJSON* detail = cJSON_GetObjectItemCaseSensitive(root, "detail");
    if (cJSON_IsString(detail) && (detail->valuestring != NULL))
    {
        COGNIT_LOG_ERROR("Error detail: %s", detail->valuestring);
    }
    else
    {
        COGNIT_LOG_INFO("No 'detail' field found in the error response.");
    }

    cJSON_Delete(root);
}



#include <faas_parser.h>
#include <cJSON.h>
#include <logger.h>
#include "cognit_encoding.h"

int8_t faasparser_parse_exec_faas_params_as_str_json(faas_t* pt_faas, uint8_t* ui8_payload_buff, size_t* payload_len)
{
    cJSON* param          = NULL;
    cJSON* params_array   = NULL;
    char* str_faas_json   = NULL;

    params_array = cJSON_CreateArray();
    if (params_array == NULL)
    {
        COGNIT_LOG_ERROR("Error creating cJSON array");
        cJSON_Delete(params_array);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    for (int i = 0; i < pt_faas->params_count; i++)
    {
        char str_b64_param[1024 * 16];
        size_t base64_len;
        param = cJSON_CreateObject();

        if (param == NULL)
        {
            COGNIT_LOG_ERROR("Error creating cJSON object");
            cJSON_Delete(params_array);
            return JSON_ERR_CODE_INVALID_JSON;
        }
        char str_param[1024];
        int param_len = pb_serialize_faas_param(pt_faas, i, (uint8_t*) str_param, sizeof(str_param));
        
        if (cognit_base64_encode((unsigned char*) str_b64_param, sizeof(str_b64_param), &base64_len, str_param, param_len) != 0) {
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
    exec_response_t t_exec_response;
    t_exec_response.res = pt_res;
    cJSON* root = cJSON_Parse(json_str);
    int i_ret = -1;

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

 cJSON* params_array = NULL;
                                   /* formato nuevo: string */
    size_t b64_len = strlen(res_item->valuestring);
    uint8_t decoded[1024 * 8];                     /* ajusta tamaño a tu caso */
    size_t json_len = 0;

    if (cognit_base64_decode(decoded, sizeof(decoded),
                             &json_len,
                             (const unsigned char*)res_item->valuestring,
                             b64_len) != 0) {
        COGNIT_LOG_ERROR("Error decoding base64 (res)");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }


    i_ret = pb_deserialize_faas_param(decoded, json_len, &t_exec_response.ret_len, t_exec_response.res);

    t_exec_response.ret_code = ret_code_item->valueint;
    t_exec_response.err_code = err_item->valueint;

    cJSON_Delete(root);

    return i_ret;
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



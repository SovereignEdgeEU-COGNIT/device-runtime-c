#include <faas_parser.h>
#include <base64.h>
#include <cJSON.h>

int8_t parse_exec_faas_params_as_str_json(exec_faas_params_t* exec_faas_params, uint8_t* ui8_payload_buff, size_t* payload_len)
{
    cJSON* root = NULL;
    cJSON* param = NULL;
    cJSON* params_array = NULL;
    char* str_faas_json = NULL;
    char* str_encoded_fc = NULL;
    char* str_param = NULL;
    char* str_b64_param = NULL;
    size_t out_fc_len = 0;
    size_t out_param_len = 0;

    root = cJSON_CreateObject();

    if (root == NULL)
    {
        printf("Error creating cJSON object\n");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }
    
    cJSON_AddStringToObject(root, "lang", "C");
    printf("strlen(exec_faas_params->fc): %ld\n", strlen(exec_faas_params->fc));
    printf("exec_faas_params->fc: %s\n", exec_faas_params->fc);
    str_encoded_fc = base64_encode((const char*)exec_faas_params->fc, strlen(exec_faas_params->fc), &out_fc_len);
    printf("str_encoded_fc: %s\n",str_encoded_fc);
    printf("strlen(str_encoded_fc): %ld\n", strlen(str_encoded_fc));
    cJSON_AddStringToObject(root, "fc", (const char*) str_encoded_fc);
    // free(str_encoded_fc);

    params_array = cJSON_CreateArray();
    if (params_array == NULL)
    {
        printf("Error creating cJSON array\n");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    for (int i = 0; i < exec_faas_params->params_count; i++)
    {
        param = cJSON_CreateObject();
        if (param == NULL)
        {
            printf("Error creating cJSON object\n");
            cJSON_Delete(root);
            return JSON_ERR_CODE_INVALID_JSON;
        }
        cJSON_AddStringToObject(param, "type", exec_faas_params->params[i].type);
        cJSON_AddStringToObject(param, "var_name", exec_faas_params->params[i].var_name);
        cJSON_AddStringToObject(param, "value", base64_encode(exec_faas_params->params[i].value, strlen(exec_faas_params->params[i].value), out_param_len));
        cJSON_AddStringToObject(param, "mode", exec_faas_params->params[i].mode);

        // Convert param to string
        str_param = cJSON_Print(param);
        // Convert param to base64
        str_b64_param = base64_encode(str_param, strlen(str_param), &out_param_len);

        cJSON_AddItemToArray(params_array, cJSON_CreateString(str_b64_param));

        free(str_param);
        free(str_b64_param);
        cJSON_Delete(param);
    }

    cJSON_AddItemToObject(root, "params", params_array);

    ui8_payload_buff = (uint8_t*)cJSON_Print(root);

    *payload_len = strlen(str_faas_json);

    cJSON_Delete(root);

    return JSON_ERR_CODE_OK;
}

int8_t parse_json_str_as_exec_faas_params(const char* json_str, exec_response_t* t_exec_response)
{
    cJSON* root = cJSON_Parse(json_str);

    if (root == NULL)
    {
        printf("Error parsing JSON\n");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    cJSON* ret_code_item = cJSON_GetObjectItem(root, "ret_code");
    cJSON* res_item = cJSON_GetObjectItem(root, "res");
    cJSON* err_item = cJSON_GetObjectItem(root, "err");

    if (!cJSON_IsNumber(ret_code_item) || !cJSON_IsString(res_item))
    {
        printf("JSON content types are wrong\n");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    t_exec_response->ret_code = ret_code_item->valueint;

    // Decode base64 res_item
    size_t out_len = 0;
    t_exec_response->res_payload = base64_decode(res_item->valuestring, strlen(res_item->valuestring), &out_len);

    if (t_exec_response->res_payload == NULL)
    {
        printf("Error decoding base64\n");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    t_exec_response->res_payload_len = out_len;
    // TODO: parse err ??

    cJSON_Delete(root);

    return JSON_ERR_CODE_OK;
}
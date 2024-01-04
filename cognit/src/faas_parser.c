#include <faas_parser.h>
#include <base64.h>
#include <cJSON.h>

int8_t faasparser_parse_exec_faas_params_as_str_json(exec_faas_params_t* exec_faas_params, uint8_t* ui8_payload_buff, size_t* payload_len)
{
    cJSON* root           = NULL;
    cJSON* param          = NULL;
    cJSON* params_array   = NULL;
    char* str_faas_json   = NULL;
    char* str_param       = NULL;
    char* str_b64_param   = NULL;
    char* str_b64_value   = NULL;
    int i_coded_value_len = 0;
    int i_coded_param_len = 0;
    int out_fc_len        = 0;

    char* str_encoded_fc = (char*)malloc(base64_encode_len(strlen(exec_faas_params->fc)));

    if (str_encoded_fc == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for encoded string\n");
    }

    root = cJSON_CreateObject();

    if (root == NULL)
    {
        printf("Error creating cJSON object\n");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    cJSON_AddStringToObject(root, "lang", "C");
    // printf("strlen(exec_faas_params->fc): %ld\n", strlen(exec_faas_params->fc));
    printf("exec_faas_params->fc: %s\n", exec_faas_params->fc);

    out_fc_len = base64_encode(str_encoded_fc, exec_faas_params->fc, strlen(exec_faas_params->fc));
    printf("str_encoded_fc: %s\n", str_encoded_fc);
    // printf("strlen(str_encoded_fc): %ld\n", strlen(str_encoded_fc));
    cJSON_AddStringToObject(root, "fc", (const char*)str_encoded_fc);
    free(str_encoded_fc);

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
            fprintf(stderr, "Error creating cJSON object\n");
            cJSON_Delete(root);
            return JSON_ERR_CODE_INVALID_JSON;
        }
        printf("exec_faas_params->params[%d].value: %s\n", i, exec_faas_params->params[i].value);

        if (exec_faas_params->params[i].value == NULL)
        {
            printf("exec_faas_params->params[%d].value is NULL\n", i);

            cJSON_AddStringToObject(param, "type", exec_faas_params->params[i].type);
            cJSON_AddStringToObject(param, "var_name", exec_faas_params->params[i].var_name);
            cJSON_AddStringToObject(param, "value", "NULL");
            cJSON_AddStringToObject(param, "mode", exec_faas_params->params[i].mode);
        }
        else
        {
            str_b64_value = malloc(base64_encode_len(strlen(exec_faas_params->params[i].value)));
            if (str_b64_value == NULL)
            {
                fprintf(stderr, "Failed to allocate memory for encoded param\n");
            }

            i_coded_value_len = base64_encode(str_b64_value, exec_faas_params->params[i].value, strlen(exec_faas_params->params[i].value));

            cJSON_AddStringToObject(param, "type", exec_faas_params->params[i].type);
            cJSON_AddStringToObject(param, "var_name", exec_faas_params->params[i].var_name);
            cJSON_AddStringToObject(param, "value", str_b64_value);
            cJSON_AddStringToObject(param, "mode", exec_faas_params->params[i].mode);
            free(str_b64_value);
        }

        // Convert param to string
        str_param = cJSON_Print(param);
        // Convert param to base64
        str_b64_param = malloc(base64_encode_len(strlen(str_param)));
        if (str_b64_param == NULL)
        {
            fprintf(stderr, "Failed to allocate memory for encoded param\n");
        }

        i_coded_param_len = base64_encode(str_b64_param, str_param, strlen(str_param));

        cJSON_AddItemToArray(params_array, cJSON_CreateString(str_b64_param));

        free(str_param);
        free(str_b64_param);
        cJSON_Delete(param);
    }

    cJSON_AddItemToObject(root, "params", params_array);
    str_faas_json = cJSON_Print(root);

    // Copy the json string to the payload buffer
    strcpy((char*)ui8_payload_buff, str_faas_json);
    *payload_len = strlen(str_faas_json);

    cJSON_Delete(root);

    return JSON_ERR_CODE_OK;
}

int8_t faasparser_parse_json_str_as_exec_response(const char* json_str, exec_response_t* t_exec_response)
{
    cJSON* root = cJSON_Parse(json_str);

    if (root == NULL)
    {
        printf("Error parsing JSON\n");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    cJSON* ret_code_item = cJSON_GetObjectItem(root, "ret_code");
    cJSON* res_item      = cJSON_GetObjectItem(root, "res");
    cJSON* err_item      = cJSON_GetObjectItem(root, "err");

    if (!cJSON_IsNumber(ret_code_item) || !cJSON_IsString(res_item))
    {
        printf("JSON content types are wrong\n");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    t_exec_response->ret_code = ret_code_item->valueint;

    // Decode base64 res_item
    int out_len = 0;

    t_exec_response->res_payload = (char*)malloc(base64_decode_len(res_item->valuestring));
    if (t_exec_response->res_payload == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for t_exec_response->res_payload\n");
    }
    out_len = base64_decode(t_exec_response->res_payload, res_item->valuestring);

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

int8_t faasparser_parse_json_str_as_async_exec_response(const char* json_str, async_exec_response_t* t_async_exec_response)
{
    cJSON* root   = cJSON_Parse(json_str);
    int8_t i8_ret = 0;

    if (root == NULL)
    {
        printf("Error parsing JSON\n");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    cJSON* status_item  = cJSON_GetObjectItem(root, "status");
    cJSON* res_item     = cJSON_GetObjectItem(root, "res");
    cJSON* exec_id_item = cJSON_GetObjectItem(root, "exec_id");

    if (!cJSON_IsString(status_item)
        || !cJSON_IsObject(exec_id_item))
    {
        printf("JSON content types are wrong\n");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    strncpy(t_async_exec_response->status, status_item->valuestring, sizeof(t_async_exec_response->status) - 1);
    // Parse exec_id as an object
    cJSON* faas_task_uuid_item = cJSON_GetObjectItem(exec_id_item, "faas_task_uuid");
    if (!cJSON_IsString(faas_task_uuid_item))
    {
        printf("faas_task_uuid is not a string\n");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    // Copy task uuid to the response struct
    strncpy(t_async_exec_response->exec_id.faas_task_uuid, faas_task_uuid_item->valuestring, sizeof(t_async_exec_response->exec_id.faas_task_uuid) - 1);

    // If res is null means srv hasnt finished the execution
    if (cJSON_IsNull(res_item))
    {
        printf("res_item is NULL\n");
        t_async_exec_response->res = NULL;
    }
    else
    {
        i8_ret = faasparser_parse_json_str_as_exec_response(cJSON_Print(res_item), t_async_exec_response->res);
        if (i8_ret != JSON_ERR_CODE_OK)
        {
            printf("Error parsing JSON\n");
            cJSON_Delete(root);
            return JSON_ERR_CODE_INVALID_JSON;
        }
    }

    cJSON_Delete(root);

    return JSON_ERR_CODE_OK;
}
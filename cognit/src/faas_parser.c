#include <faas_parser.h>
#include <base64.h>
#include <cJSON.h>
#include <logger.h>
#include "mbedtls/sha256.h"
#include "mbedtls/compat-2.x.h"
#include "mbedtls/base64.h"

static void str_to_hex(unsigned char *hash, unsigned int longitud_hash, char *hash_hex) {
    for (unsigned int i = 0; i < longitud_hash; i++) {
        sprintf(&hash_hex[i * 2], "%02x", hash[i]);
    }
    hash_hex[longitud_hash * 2] = '\0'; // Agregar terminador de cadena
}

int8_t faasparser_parse_fc_as_str_json(faas_t* pt_faas, uint8_t* ui8_payload_buff, size_t* payload_len)
{
    cJSON* root           = NULL;
    cJSON* param          = NULL;
    cJSON* params_array   = NULL;
    char* str_faas_json   = NULL;
    int i_coded_param_len = 0;
    unsigned char str_encoded_fc[1024 * 16];
    size_t out_fc_len = 0;
    unsigned char hash[32];
    char str_hash[2048];
    char fc_hex[1024 * 16];
    
    root = cJSON_CreateObject();

    if (root == NULL)
    {
        COGNIT_LOG_ERROR("Error creating cJSON object");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    cJSON_AddStringToObject(root, "LANG", "C");

    uint8_t ui8_payload[1024 * 16];
    int fc_len = pb_serialize_fc(pt_faas, ui8_payload, sizeof(ui8_payload));

    if (mbedtls_base64_encode(str_encoded_fc, sizeof(str_encoded_fc), &out_fc_len, ui8_payload, fc_len) != 0) {
        COGNIT_LOG_ERROR("Error coding in base64s");
        return JSON_ERR_CODE_INVALID_JSON;
    }

    // COGNIT_LOG_DEBUG("strlen(str_encoded_fc): %ld", strlen(str_encoded_fc));
    cJSON_AddStringToObject(root, "FC", (const char*)str_encoded_fc);
    
    str_to_hex(pt_faas->myfunc.fc_code, strlen(pt_faas->myfunc.fc_code), fc_hex);
    
    if (mbedtls_sha256_ret((const unsigned char *)fc_hex, strlen(fc_hex), hash, 0) != 0) {
        COGNIT_LOG_ERROR("Error calculating hash SHA-256");
        return JSON_ERR_CODE_INVALID_JSON;
    }
    
    str_to_hex(hash, strlen(hash), str_hash);
    
    cJSON_AddStringToObject(root, "FC_HASH", str_hash);

    str_faas_json = cJSON_Print(root);

    // Copy the json string to the payload buffer
    strcpy((char*)ui8_payload_buff, str_faas_json);
    *payload_len = strlen(str_faas_json);

    cJSON_Delete(root);
    free(str_faas_json);

    return JSON_ERR_CODE_OK;
}

int8_t faasparser_parse_exec_faas_params_as_str_json(faas_t* pt_faas, uint8_t* ui8_payload_buff, size_t* payload_len)
{
    cJSON* root           = NULL;
    cJSON* param          = NULL;
    cJSON* params_array   = NULL;
    char* str_faas_json   = NULL;
    char* str_param       = NULL;
    char* str_b64_param   = NULL;
    int i_coded_param_len = 0;
    int out_params_len        = 0;

    root = cJSON_CreateObject();

    if (root == NULL)
    {
        COGNIT_LOG_ERROR("Error creating cJSON object");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    params_array = cJSON_CreateArray();
    if (params_array == NULL)
    {
        COGNIT_LOG_ERROR("Error creating cJSON array");
        cJSON_Delete(root);
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
            cJSON_Delete(root);
            return JSON_ERR_CODE_INVALID_JSON;
        }
        uint8_t str_param[1024 * 16];
        int param_len = pb_serialize_faas_param(pt_faas, i, str_param, sizeof(str_param));
        
        if (mbedtls_base64_encode(str_b64_param, sizeof(str_b64_param), &base64_len, str_param, param_len) != 0) {
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

    cJSON_Delete(root);
    free(str_faas_json);

    return JSON_ERR_CODE_OK;
}

int8_t faasparser_parse_json_str_as_exec_response(const char* json_str, exec_response_t* pt_exec_response)
{
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

    pt_exec_response->ret_code = ret_code_item->valueint;

    // Decode base64 res_item
    int out_len = 0;

    char* res_payload = (char*)malloc(base64_decode_len(res_item->valuestring));
    if (res_payload == NULL)
    {
        COGNIT_LOG_ERROR("Failed to allocate memory for res_payload");
    }
    out_len = base64_decode(res_payload, res_item->valuestring);

    uint8_t i8_ret = pb_deserialize_faas_param(res_payload, out_len, &pt_exec_response->res);

    if (pt_exec_response->res == NULL || i8_ret == -1)
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



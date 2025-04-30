#include <cf_parser.h>
#include "mbedtls/compat-2.x.h"
#include <mbedtls/base64.h>
#include <mbedtls/sha256.h>
#include <cJSON.h>
#include <logger.h>

static void str_to_hex(unsigned char *hash, unsigned int longitud_hash, char *hash_hex) {
    for (unsigned int i = 0; i < longitud_hash; i++) {
        sprintf(&hash_hex[i * 2], "%02x", hash[i]);
    }
    hash_hex[longitud_hash * 2] = '\0'; // Agregar terminador de cadena
}

void cfparser_parse_str_response_as_token(char* token, uint8_t* ui8_payload)
{
    int len = strlen(ui8_payload);
    if (len > 1 && ui8_payload[0] == '"' && ui8_payload[len - 1] == '"')
    {
        // Mueve todos los caracteres una posición hacia atrás
        memmove(token, ui8_payload + 1, len - 1);
        // Reemplaza la última comilla por el terminador nulo
        token[len - 2] = '\0';
    }
}

int8_t cfparser_parse_requirements_as_str_json(scheduling_t* t_app_requirements, uint8_t* ui8_payload_buff, size_t* payload_len)
{
    cJSON* root = NULL;

    char* str_sr_json = NULL;

    root = cJSON_CreateObject();
    if (root == NULL)
    {
        COGNIT_LOG_ERROR("Error creating cJSON object");
        return JSON_ERR_CODE_INVALID_JSON;
    }

    if (t_app_requirements->flavour != NULL)
    {
        cJSON_AddStringToObject(root, "FLAVOUR", t_app_requirements->flavour);
    }
    else
    {
        COGNIT_LOG_ERROR("Flavour not specified");
        cJSON_Delete(root);
        return -1;
    }

    if (t_app_requirements->max_latency > 0)
    {
        cJSON_AddNumberToObject(root, "MAX_LATENCY", t_app_requirements->max_latency);
    }

    if (t_app_requirements->max_function_execution_time > 0)
    {
        cJSON_AddNumberToObject(root, "MAX_FUNCTION_EXECUTION_TIME", t_app_requirements->max_function_execution_time);
    }

    if (t_app_requirements->min_renewable > 0)
    {
        cJSON_AddNumberToObject(root, "MIN_RENEWABLE_USAGE", t_app_requirements->min_renewable);
    }

    if (t_app_requirements->max_latency > 0)
    {
        if (t_app_requirements->geolocation == NULL)
        {
            COGNIT_LOG_ERROR("Geolocation is required if MAX_LATENCY is defined");
            cJSON_Delete(root);
            return -1;
        }
    }

    if (t_app_requirements->geolocation[0] != '\0')
    {
        cJSON_AddStringToObject(root, "GEOLOCATION", t_app_requirements->geolocation);
    }

    str_sr_json = cJSON_Print(root);

    // Copy the json string to the payload buffer
    strcpy((char*)ui8_payload_buff, str_sr_json);
    *payload_len = strlen(str_sr_json);

    free(str_sr_json);
    cJSON_Delete(root);

    return JSON_ERR_CODE_OK;
}

int8_t cfparser_parse_json_str_as_ecf_address(const char* json_str, ecf_response_t* pt_ecf_res)
{
    cJSON* json_array = cJSON_Parse(json_str);
    cJSON* root       = cJSON_GetArrayItem(json_array, 0);

    if (root == NULL)
    {
        COGNIT_LOG_ERROR("Error parsing JSON");
        cJSON_Delete(json_array);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    cJSON* cname_item             = cJSON_GetObjectItem(root, "NAME");
    cJSON* ui32_id_item           = cJSON_GetObjectItem(root, "ID");
    cJSON* hosts_item             = cJSON_GetObjectItem(root, "HOSTS");
    cJSON* datastores_item        = cJSON_GetObjectItem(root, "DATASTORES");
    cJSON* scheduling_config_item = cJSON_GetObjectItem(root, "VNETS");
    cJSON* template_item          = cJSON_GetObjectItem(root, "TEMPLATE");

    if (!cJSON_IsNumber(ui32_id_item) || !cJSON_IsString(cname_item))
    {
        COGNIT_LOG_ERROR("JSON content types are wrong");
        cJSON_Delete(json_array);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    //Name
    strcpy(pt_ecf_res->name, cname_item->valuestring);

    //id
    pt_ecf_res->id = ui32_id_item->valueint;

    //device_info;
    if (template_item != NULL)
    {
        cJSON* ecf_item = cJSON_GetObjectItem(template_item, "EDGE_CLUSTER_FRONTEND");
        if (ecf_item != NULL)
            strcpy(pt_ecf_res->template, ecf_item->valuestring);
    }

    cJSON_Delete(json_array);

    return JSON_ERR_CODE_OK;
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
    
    str_to_hex(hash, 32, str_hash);
    
    cJSON_AddStringToObject(root, "FC_HASH", str_hash);

    str_faas_json = cJSON_Print(root);

    // Copy the json string to the payload buffer
    strcpy((char*)ui8_payload_buff, str_faas_json);
    *payload_len = strlen(str_faas_json);

    cJSON_Delete(root);
    free(str_faas_json);

    return JSON_ERR_CODE_OK;
}


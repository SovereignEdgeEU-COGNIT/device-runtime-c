#include <cf_parser.h>
#include <base64.h>
#include <cJSON.h>
#include <logger.h>

void cfparser_parse_str_response_as_token(char* token, uint8_t *ui8_payload){
    int len = strlen(ui8_payload);
    if (len > 1 && ui8_payload[0] == '"' && ui8_payload[len - 1] == '"') {
        // Mueve todos los caracteres una posición hacia atrás
        memmove(token, ui8_payload + 1, len - 1);
        // Reemplaza la última comilla por el terminador nulo
        token[len - 2] = '\0';
    }
}

int8_t cfparser_parse_requirements_as_str_json(scheduling_t* t_app_requirements, uint8_t* ui8_payload_buff, size_t* payload_len)
{
    cJSON* root        = NULL;
    
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
    
    if (t_app_requirements->geolocation[0] != NULL)
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
    cJSON* json_array   = cJSON_Parse(json_str);
    cJSON* root         = cJSON_GetArrayItem(json_array, 0);

    if (root == NULL)
    {
        COGNIT_LOG_ERROR("Error parsing JSON");
        return JSON_ERR_CODE_INVALID_JSON;
    }

    cJSON* cname_item             = cJSON_GetObjectItem(root, "NAME");
    cJSON* ui32_id_item           = cJSON_GetObjectItem(root, "ID");
    cJSON* hosts_item             = cJSON_GetObjectItem(root, "HOSTS");
    cJSON* datastores_item        = cJSON_GetObjectItem(root, "DATASTORES");
    cJSON* scheduling_config_item = cJSON_GetObjectItem(root, "VNETS");
    cJSON* template_item       = cJSON_GetObjectItem(root, "TEMPLATE");

    if (!cJSON_IsNumber(ui32_id_item) || !cJSON_IsString(cname_item))
    {
        COGNIT_LOG_ERROR("JSON content types are wrong");
        cJSON_Delete(root);
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

    cJSON_Delete(root);

    return JSON_ERR_CODE_OK;
}

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

/*
int8_t srparser_parse_json_str_as_cognit_frontend(const char* json_str, cognit_frontend_cli_t* pt_cfc)
{
    cJSON* root   = cJSON_Parse(json_str);

    if (root == NULL)
    {
        COGNIT_LOG_ERROR("Error parsing JSON");
        return JSON_ERR_CODE_INVALID_JSON;
    }

    cJSON* cname_item                 = cJSON_GetObjectItem(root, "NAME");
    cJSON* ui32_id_item           = cJSON_GetObjectItem(root, "ID");
    cJSON* faas_config_item       = cJSON_GetObjectItem(root, "HOSTS");
    cJSON* daas_config_item       = cJSON_GetObjectItem(root, "DATASTORES");
    cJSON* scheduling_config_item = cJSON_GetObjectItem(root, "VNETS");
    cJSON* device_info_item       = cJSON_GetObjectItem(root, "TEMPLATE");

    if (!cJSON_IsNumber(ui32_id_item) || !cJSON_IsString(cname_item))
    {
        COGNIT_LOG_ERROR("JSON content types are wrong");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    //Name
    //  strcpy(pt_cfc->c_name, cname_item->valuestring);

    //id
    COGNIT_LOG_DEBUG("%d%d", pt_cfc->, ui32_id_item->valueint);
    pt_cfc->ui32_id = ui32_id_item->valueint;

    //faas_config
    if (faas_config_item != NULL)
    {
        cJSON* faas_cpu_item = cJSON_GetObjectItem(faas_config_item, "CPU");
        if (faas_cpu_item != NULL)
        {
            pt_cfc->faas_config.ui8_cpu = faas_cpu_item->valueint;
        }

        cJSON* faas_memory_item = cJSON_GetObjectItem(faas_config_item, "MEMORY");
        if (faas_memory_item != NULL)
        {
            pt_cfc->faas_config.ui32_memory = faas_memory_item->valueint;
        }

        cJSON* faas_disk_size_item = cJSON_GetObjectItem(faas_config_item, "DISK_SIZE");
        if (faas_disk_size_item != NULL)
        {
            pt_cfc->faas_config.ui32_disk_size = faas_disk_size_item->valueint;
        }

        cJSON* faas_flavour_item = cJSON_GetObjectItem(faas_config_item, "FLAVOUR");
        strcpy(pt_cfc->faas_config.c_flavour, faas_flavour_item->valuestring);

        cJSON* faas_endpoint_item = cJSON_GetObjectItem(faas_config_item, "ENDPOINT");
        if (faas_endpoint_item != NULL)
        {
            strcpy(pt_cfc->faas_config.c_endpoint, faas_endpoint_item->valuestring);
            COGNIT_LOG_DEBUG("pt_cfc->faas_config.c_endpoint: %s", pt_cfc->faas_config.c_endpoint);
        }

        cJSON* faas_state_item = cJSON_GetObjectItem(faas_config_item, "STATE");
        strcpy(pt_cfc->faas_config.c_state, faas_state_item->valuestring);

        cJSON* faas_vm_id_item = cJSON_GetObjectItem(faas_config_item, "VM_ID");
        if (faas_vm_id_item != NULL)
        {
            pt_cfc->faas_config.ui32_vm_id = faas_vm_id_item->valueint;
        }
    }
    else
    {
        COGNIT_LOG_ERROR("JSON message is wrong");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    //daas_config
    if (daas_config_item != NULL)
    {
        cJSON* daas_cpu_item = cJSON_GetObjectItem(daas_config_item, "CPU");
        if (daas_cpu_item != NULL)
        {
            pt_cfc->daas_config.ui8_cpu = daas_cpu_item->valueint;
        }
        cJSON* daas_memory_item = cJSON_GetObjectItem(daas_config_item, "MEMORY");
        if (daas_memory_item != NULL)
        {
            pt_cfc->daas_config.ui32_memory = daas_memory_item->valueint;
        }

        cJSON* daas_disk_size_item = cJSON_GetObjectItem(daas_config_item, "DISK_SIZE");
        if (daas_disk_size_item != NULL)
        {
            char* end;
            pt_cfc->daas_config.ui32_disk_size = (uint32_t)strtoul(daas_disk_size_item->valuestring, &end, 10);
            // pt_cfc->daas_config.ui32_disk_size = daas_disk_size_item->valuestring;
        }

        cJSON* daas_flavour_item = cJSON_GetObjectItem(daas_config_item, "FLAVOUR");
        strcpy(pt_cfc->daas_config.c_flavour, daas_flavour_item->valuestring);

        cJSON* daas_endpoint_item = cJSON_GetObjectItem(daas_config_item, "ENDPOINT");
        if (daas_endpoint_item != NULL)
        {
            strcpy(pt_cfc->daas_config.c_endpoint, daas_endpoint_item->valuestring);
        }

        cJSON* daas_state_item = cJSON_GetObjectItem(daas_config_item, "STATE");
        strcpy(pt_cfc->daas_config.c_state, daas_state_item->valuestring);

        cJSON* daas_vm_id_item = cJSON_GetObjectItem(daas_config_item, "VM_ID");
        if (daas_vm_id_item != NULL)
        {
            pt_cfc->daas_config.ui32_vm_id = daas_vm_id_item->valueint;
        }
    }

    //scheduling_config;
    if (scheduling_config_item != NULL)
    {
        cJSON* scheduling_policy_item = cJSON_GetObjectItem(scheduling_config_item, "POLICY");
        strcpy(pt_cfc->scheduling_config.c_policy, scheduling_policy_item->valuestring);

        cJSON* scheduling_req_item = cJSON_GetObjectItem(daas_config_item, "REQUIREMENTS");
        if (scheduling_req_item != NULL)
            strcpy(pt_cfc->scheduling_config.c_requirements, scheduling_policy_item->valuestring);
    }

    //device_info;
    if (device_info_item != NULL)
    {
        cJSON* device_latency_item = cJSON_GetObjectItem(device_info_item, "LATENCY_TO_PE");
        if (device_latency_item != NULL)
            pt_cfc->device_info.ui32_latency_to_pe = device_latency_item->valueint;

        cJSON* device_loc_item = cJSON_GetObjectItem(device_info_item, "GEOGRAPHIC_LOCATION");
        strcpy(pt_cfc->device_info.c_geograph_location, device_loc_item->valuestring);
    }

    cJSON_Delete(sr);

    return JSON_ERR_CODE_OK;
}
*/
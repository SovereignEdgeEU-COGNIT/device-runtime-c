#include <sr_parser.h>
#include <base64.h>
#include <cJSON.h>
#include <logger.h>

int8_t  srparser_parse_serverless_runtime_as_str_json(serverless_runtime_t* t_serverless_runtime, uint8_t* ui8_payload_buff, size_t* payload_len)
{
    cJSON* root           = NULL;
    cJSON* faas           = NULL;
    cJSON* daas           = NULL;
    cJSON* scheduling     = NULL;
    cJSON* device_info    = NULL;

    char* str_sr_json   = NULL;

    root = cJSON_CreateObject();

    if (root == NULL)
    {
        COGNIT_LOG_ERROR("Error creating cJSON object");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    if(t_serverless_runtime->c_name != NULL){
        cJSON_AddStringToObject(root, "name", t_serverless_runtime->c_name);
        COGNIT_LOG_DEBUG("t_serverless_runtime->c_name: %s", t_serverless_runtime->c_name);
    }

    if(t_serverless_runtime->ui32_id != NULL){
        cJSON_AddNumberToObject(root, "id", t_serverless_runtime->ui32_id);
        COGNIT_LOG_DEBUG("t_serverless_runtime->ui32_id: %d", t_serverless_runtime->ui32_id);
    }
    //faas

    faas = cJSON_CreateObject();
    if (faas == NULL)
    {
        COGNIT_LOG_ERROR("Error creating cJSON object");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    if(t_serverless_runtime->faas_config.ui8_cpu != NULL){
        cJSON_AddNumberToObject(faas, "cpu", t_serverless_runtime->faas_config.ui8_cpu);
    }

    if(t_serverless_runtime->faas_config.ui32_memory != NULL){
        cJSON_AddNumberToObject(faas, "memory", t_serverless_runtime->faas_config.ui32_memory);
    }

    if(t_serverless_runtime->faas_config.ui32_disk_size != NULL){
        cJSON_AddNumberToObject(faas, "disk_size", t_serverless_runtime->faas_config.ui32_disk_size);
    }

    if(t_serverless_runtime->faas_config.c_flavour[0] != NULL){
        cJSON_AddStringToObject(faas, "flavour", t_serverless_runtime->faas_config.c_flavour);
    }

    if(t_serverless_runtime->faas_config.c_endpoint[0] != NULL){
        cJSON_AddStringToObject(faas, "endpoint", t_serverless_runtime->faas_config.c_endpoint);
    }

    if(t_serverless_runtime->faas_config.c_state[0] != NULL){
        cJSON_AddStringToObject(faas, "state", t_serverless_runtime->faas_config.c_state);
    }
    
    if(t_serverless_runtime->faas_config.c_vm_id[0] != NULL){
        cJSON_AddStringToObject(faas, "vm_id", t_serverless_runtime->faas_config.c_vm_id);
    }

    cJSON_AddItemToObject(root, "faas", faas);

    if(t_serverless_runtime->daas_config.c_flavour[0] != NULL && t_serverless_runtime->daas_config.c_state[0] != NULL){
        daas = cJSON_CreateObject();
    if (daas == NULL)
    {
        COGNIT_LOG_ERROR("Error creating cJSON object");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    cJSON_AddStringToObject(daas, "flavour", t_serverless_runtime->daas_config.c_flavour);
    cJSON_AddStringToObject(daas, "state", t_serverless_runtime->daas_config.c_state);
    
    if(t_serverless_runtime->daas_config.ui8_cpu != NULL){
        cJSON_AddNumberToObject(daas, "cpu", t_serverless_runtime->daas_config.ui8_cpu);
    }

    if(t_serverless_runtime->daas_config.ui32_memory != NULL){
        cJSON_AddNumberToObject(daas, "memory", t_serverless_runtime->daas_config.ui32_memory);
    }

    if(t_serverless_runtime->daas_config.ui32_disk_size != NULL){
        cJSON_AddNumberToObject(daas, "disk_size", t_serverless_runtime->daas_config.ui32_disk_size);
    }

    if(t_serverless_runtime->daas_config.c_endpoint[0] != NULL){
        cJSON_AddStringToObject(daas, "endpoint", t_serverless_runtime->daas_config.c_endpoint);
    }

    if(t_serverless_runtime->daas_config.c_vm_id[0] != NULL){
        cJSON_AddStringToObject(daas, "vm_id", t_serverless_runtime->daas_config.c_vm_id);
    }

    cJSON_AddItemToObject(root, "daas", daas);
    }


    if(t_serverless_runtime->scheduling_config.c_policy[0] != NULL && t_serverless_runtime->scheduling_config.c_requirements[0] != NULL){
        
        scheduling = cJSON_CreateObject();
        if (scheduling == NULL)
        {
            COGNIT_LOG_ERROR("Error creating cJSON object");
            cJSON_Delete(root);
            return JSON_ERR_CODE_INVALID_JSON;
        }
        cJSON_AddStringToObject(scheduling, "policy", t_serverless_runtime->scheduling_config.c_policy);
        cJSON_AddStringToObject(scheduling, "requirements", t_serverless_runtime->scheduling_config.c_requirements);
    
    cJSON_AddItemToObject(root, "scheduling", scheduling);
    }

    if(t_serverless_runtime->device_info.ui32_latency_to_pe != NULL && t_serverless_runtime->device_info.c_geograph_location[0] != NULL){
        cJSON_AddNumberToObject(device_info, "latency_to_pe", t_serverless_runtime->device_info.ui32_latency_to_pe);
        cJSON_AddStringToObject(device_info, "geiographic_location", t_serverless_runtime->device_info.c_geograph_location);
    
    
        device_info = cJSON_CreateObject();
        if (device_info == NULL)
        {
            COGNIT_LOG_ERROR("Error creating cJSON object");
            cJSON_Delete(root);
            return JSON_ERR_CODE_INVALID_JSON;
        }

        cJSON_AddItemToObject(root, "device_info", device_info);
    }

    str_sr_json = cJSON_Print(root);

    // Copy the json string to the payload buffer
    strcpy((char*)ui8_payload_buff, str_sr_json);
    *payload_len = strlen(str_sr_json);

    cJSON_Delete(root);

    return JSON_ERR_CODE_OK;
}

int8_t srparser_parse_json_str_as_serverless_runtime(const char* json_str, serverless_runtime_t* t_serverless_runtime)
{
    cJSON* root = cJSON_Parse(json_str);

    if (root == NULL)
    {
        COGNIT_LOG_ERROR("Error parsing JSON");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    cJSON* cname_item = cJSON_GetObjectItem(root, "name");
    cJSON* ui32_id_item      = cJSON_GetObjectItem(root, "id");
    cJSON* faas_config_item      = cJSON_GetObjectItem(root, "faas");
    cJSON* daas_config_item      = cJSON_GetObjectItem(root, "daas");
    cJSON* scheduling_config_item      = cJSON_GetObjectItem(root, "scheduling");
    cJSON* device_info_item      = cJSON_GetObjectItem(root, "device_info");

    if (!cJSON_IsNumber(ui32_id_item) || !cJSON_IsString(cname_item))
    {
        COGNIT_LOG_ERROR("JSON content types are wrong");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    //Name
    strcpy(t_serverless_runtime->c_name, cname_item->valuestring);

    COGNIT_LOG_DEBUG("serverless runtime name: %s", t_serverless_runtime->c_name);
    
    //id
    t_serverless_runtime->ui32_id = ui32_id_item->valueint;
    COGNIT_LOG_DEBUG("serverless runtime id: %d", t_serverless_runtime->ui32_id);


    //faas_config
    if (faas_config_item != NULL)
    {
        cJSON* faas_cpu_item      = cJSON_GetObjectItem(faas_config_item, "cpu");
        if (faas_cpu_item != NULL)
        {
            t_serverless_runtime->faas_config.ui8_cpu = faas_cpu_item->valueint;
        }
        cJSON* faas_memory_item      = cJSON_GetObjectItem(faas_config_item, "memory");
        if (faas_memory_item != NULL)
        {
            t_serverless_runtime->faas_config.ui32_memory = faas_memory_item->valueint;
        }
        
        cJSON* faas_disk_size_item      = cJSON_GetObjectItem(faas_config_item, "disk_size");
        if (faas_disk_size_item != NULL)
        {
            t_serverless_runtime->faas_config.ui32_disk_size = faas_disk_size_item->valueint;
        }
        
        cJSON* faas_flavour_item      = cJSON_GetObjectItem(faas_config_item, "flavour");
        strcpy(t_serverless_runtime->faas_config.c_flavour , faas_flavour_item->valuestring);

        cJSON* faas_endpoint_item      = cJSON_GetObjectItem(faas_config_item, "endpoint");
        if (faas_endpoint_item != NULL)
        {
            strcpy(t_serverless_runtime->faas_config.c_endpoint,faas_endpoint_item->valuestring);
        }
        
        cJSON* faas_state_item      = cJSON_GetObjectItem(faas_config_item, "state");
        strcpy(t_serverless_runtime->faas_config.c_state,faas_state_item->valuestring);
        
        cJSON* faas_vm_id_item      = cJSON_GetObjectItem(faas_config_item, "vm_id");
        if (faas_vm_id_item != NULL)
        {
            strcpy(t_serverless_runtime->faas_config.c_vm_id,faas_vm_id_item->valuestring);
        }
    }
    else{
        COGNIT_LOG_ERROR("JSON message is wrong");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }
    
    //daas_config
    if (daas_config_item != NULL)
    {
        cJSON* daas_cpu_item      = cJSON_GetObjectItem(daas_config_item, "cpu");
        if (daas_cpu_item != NULL)
        {
            t_serverless_runtime->daas_config.ui8_cpu = daas_cpu_item->valueint;
        }
        cJSON* daas_memory_item      = cJSON_GetObjectItem(daas_config_item, "memory");
        if (daas_memory_item != NULL)
        {
            t_serverless_runtime->daas_config.ui32_memory = daas_memory_item->valueint;
        }
        
        cJSON* daas_disk_size_item      = cJSON_GetObjectItem(daas_config_item, "disk_size");
        if (daas_disk_size_item != NULL)
        {
            t_serverless_runtime->daas_config.ui32_disk_size = daas_disk_size_item->valuestring;
        }
        
        cJSON* daas_flavour_item      = cJSON_GetObjectItem(daas_config_item, "flavour");
        strcpy(t_serverless_runtime->daas_config.c_flavour,daas_flavour_item->valuestring);

        cJSON* daas_endpoint_item      = cJSON_GetObjectItem(daas_config_item, "endpoint");
        if (daas_endpoint_item != NULL)
        {
            strcpy(t_serverless_runtime->daas_config.c_endpoint,daas_endpoint_item->valuestring);
        }
        
        cJSON* daas_state_item      = cJSON_GetObjectItem(daas_config_item, "state");
        strcpy(t_serverless_runtime->daas_config.c_state,daas_state_item->valuestring);
        
        cJSON* daas_vm_id_item      = cJSON_GetObjectItem(daas_config_item, "vm_id");
        if (daas_vm_id_item != NULL)
        {
            strcpy(t_serverless_runtime->daas_config.c_vm_id,daas_vm_id_item->valuestring);
        }
    }

    //scheduling_config;
    if (scheduling_config_item != NULL)
    {
        cJSON* scheduling_policy_item      = cJSON_GetObjectItem(scheduling_config_item, "policy");
        strcpy(t_serverless_runtime->scheduling_config.c_policy,scheduling_policy_item->valuestring);

        cJSON* scheduling_req_item      = cJSON_GetObjectItem(daas_config_item, "requirements");
        strcpy(t_serverless_runtime->scheduling_config.c_requirements,scheduling_policy_item->valuestring);
    }

    //device_info;
    if (device_info_item != NULL)
    {
        cJSON* device_latency_item      = cJSON_GetObjectItem(device_info_item, "latency_to_pe");
        t_serverless_runtime->device_info.ui32_latency_to_pe = device_latency_item->valueint;

        cJSON* device_loc_item      = cJSON_GetObjectItem(device_info_item, "geographic_location");
        strcpy(t_serverless_runtime->device_info.c_geograph_location,device_loc_item->valuestring);
    }

    cJSON_Delete(root);

    return JSON_ERR_CODE_OK;
}

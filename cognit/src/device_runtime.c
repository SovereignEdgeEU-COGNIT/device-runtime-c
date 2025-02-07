#include <device_runtime.h>
#include <string.h>
#include <yaml.h>
#include <logger.h>

static int read_yaml_config(const char *filename, cognit_config_t *config) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        COGNIT_LOG_ERROR("Could not open file %s", filename);
        return -1;
    }

    yaml_parser_t parser;
    yaml_event_t event;
    char *key = NULL;
    int done = 0;

    if (!yaml_parser_initialize(&parser)) {
        COGNIT_LOG_ERROR("Could not init YAML parser");
        fclose(file);
        return -1;
    }

    yaml_parser_set_input_file(&parser, file);

    while (!done) {
        if (!yaml_parser_parse(&parser, &event)) {
            COGNIT_LOG_ERROR("Parsing error in line %ld, column %ld: %s\n",
                              parser.problem_mark.line + 1, parser.problem_mark.column + 1,
                              parser.problem);
            break;
        }

        switch (event.type) {
            case YAML_SCALAR_EVENT:
                if (key == NULL) {
                    key = strdup((char *)event.data.scalar.value);
                } else {
                    if (strcmp(key, "api_endpoint") == 0) {
                        config->cognit_frontend_endpoint = strdup((char *)event.data.scalar.value);
                    } else if (strcmp(key, "credentials") == 0) {
                        char *credentials = strdup((char *)event.data.scalar.value);
                        char *usr = strtok(credentials, ":");
                        char *pwd = strtok(NULL, ":");
                        if (usr && pwd) {
                            config->cognit_frontend_usr = strdup(usr);
                            config->cognit_frontend_pwd = strdup(pwd);
                        }
                        free(credentials);
                    } else if (strcmp(key, "local_endpoint") == 0) {
                        config->local_endpoint = strdup((char *)event.data.scalar.value);
                    }
                    free(key);
                    key = NULL;
                }
                break;
            case YAML_STREAM_END_EVENT:
                done = 1;
                break;
            default:
                break;
        }

        yaml_event_delete(&event);
    }

    yaml_parser_delete(&parser);
    fclose(file);
    return 0;
}

static void free_config(cognit_config_t *config)
{
    free((void *)config->cognit_frontend_endpoint);
    free((void *)config->cognit_frontend_usr);
    free((void *)config->cognit_frontend_pwd);
}

e_status_code_t device_runtime_init(device_runtime_t* pt_dr, char* config_path, scheduling_t t_reqs, faas_t *pt_faas)
{
    if(pt_dr == NULL)
    {
        COGNIT_LOG_ERROR("Device runtime not initialized");
        return E_ST_CODE_ERROR;
    }

    memset(pt_dr, 0, sizeof(device_runtime_t));

    if (read_yaml_config(config_path, &pt_dr->m_t_config) == 0) {
        COGNIT_LOG_DEBUG("Cognit Frontend Endpoint: %s", pt_dr->m_t_config.cognit_frontend_endpoint);
        COGNIT_LOG_DEBUG("Cognit Frontend Username: %s", pt_dr->m_t_config.cognit_frontend_usr);
        COGNIT_LOG_DEBUG("Cognit Frontend Password: %s", pt_dr->m_t_config.cognit_frontend_pwd);
    } else {
        return E_ST_CODE_ERROR;
    }

    int ret = dr_state_machine_init(&pt_dr->m_t_device_runtime_sm, pt_dr->m_t_config, pt_faas);

    dr_sm_update_requirements(&pt_dr->m_t_device_runtime_sm, t_reqs);
    
    return ret;
}


e_status_code_t device_runtime_call(device_runtime_t* pt_dr, faas_t *pt_faas, scheduling_t t_new_reqs, void** pt_exec_response)
{
    if(pt_dr == NULL)
    {
        COGNIT_LOG_ERROR("Device runtime state machine not initialized");
        return E_ST_CODE_ERROR;
    }

    dr_sm_update_requirements(&pt_dr->m_t_device_runtime_sm, t_new_reqs);

    dr_sm_offload_function(&pt_dr->m_t_device_runtime_sm, pt_faas, pt_exec_response);
    //Handle response
    if(*pt_exec_response == NULL)
    {
        COGNIT_LOG_ERROR("Response not obtailed");
        return E_ST_CODE_ERROR;
    }
    
    return E_ST_CODE_SUCCESS;
}







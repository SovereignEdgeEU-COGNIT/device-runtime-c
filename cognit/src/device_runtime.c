#include <device_runtime.h>
#include <string.h>
#include <yaml.h>
#include <logger.h>

static int read_yaml_config(const char *filename, cognit_config_t *config) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: No se pudo abrir el archivo %s\n", filename);
        return -1;
    }

    yaml_parser_t parser;
    yaml_event_t event;
    char *key = NULL;
    int done = 0;

    if (!yaml_parser_initialize(&parser)) {
        fprintf(stderr, "Error: No se pudo inicializar el parser YAML\n");
        fclose(file);
        return -1;
    }

    yaml_parser_set_input_file(&parser, file);

    while (!done) {
        if (!yaml_parser_parse(&parser, &event)) {
            fprintf(stderr, "Error de parsing en línea %ld, columna %ld: %s\n",
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

e_status_code_t device_runtime_init(device_runtime_t* pt_dr, char* config_path, scheduling_t t_reqs)
{
    if (read_yaml_config(config_path, &pt_dr->m_t_config) == 0) {
        COGNIT_LOG_DEBUG("Cognit Frontend Endpoint: %s\n", pt_dr->m_t_config.cognit_frontend_endpoint);
        COGNIT_LOG_DEBUG("Cognit Frontend Username: %s\n", pt_dr->m_t_config.cognit_frontend_usr);
        COGNIT_LOG_DEBUG("Cognit Frontend Password: %s\n", pt_dr->m_t_config.cognit_frontend_pwd);
    } else {
        COGNIT_LOG_ERROR("Error reading config file\n");
        return E_ST_CODE_ERROR;
    }

    pt_dr->m_t_requirements = t_reqs; ////////

    int ret = device_runtime_sm_init(&pt_dr->m_t_device_runtime_sm, pt_dr->m_t_config, pt_dr->m_t_requirements);
    
    return ret;
}


e_status_code_t device_runtime_call(device_runtime_t* pt_dr, exec_response_t* pt_exec_response)
{
    return E_ST_CODE_SUCCESS;
}

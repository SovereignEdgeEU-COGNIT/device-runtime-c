#include <faas_parser.h>
#include <base64.h>
#include <cJSON.h>
#include <logger.h>
#include <openssl/evp.h>

static void hash_a_hex(unsigned char *hash, unsigned int longitud_hash, char *hash_hex) {
    for (unsigned int i = 0; i < longitud_hash; i++) {
        sprintf(&hash_hex[i * 2], "%02x", hash[i]);
    }
    hash_hex[longitud_hash * 2] = '\0'; // Agregar terminador de cadena
}

static int calc_hash(char* cadena, unsigned char* hash, unsigned int *pt_hash_len) {
    // Cadena a hashear
    //const char *cadena = "Hola Mundo";
    // Buffer para almacenar el hash
    unsigned int longitud_hash;

    // Crear un contexto para el digest
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (ctx == NULL) {
        fprintf(stderr, "Error al crear el contexto\n");
        return 1;
    }

    // Inicializar el contexto para SHA-256
    if(1 != EVP_DigestInit_ex(ctx, EVP_sha256(), NULL)) {
        fprintf(stderr, "Error al inicializar SHA-256\n");
        EVP_MD_CTX_free(ctx);
        return 1;
    }

    // Actualizar el digest con la cadena
    if(1 != EVP_DigestUpdate(ctx, cadena, strlen(cadena))) {
        fprintf(stderr, "Error al actualizar el digest\n");
        EVP_MD_CTX_free(ctx);
        return 1;
    }

    // Finalizar el digest y obtener el resultado
    if(1 != EVP_DigestFinal_ex(ctx, hash, &longitud_hash)) {
        fprintf(stderr, "Error al finalizar el digest\n");
        EVP_MD_CTX_free(ctx);
        return 1;
    }

    // Liberar el contexto
    EVP_MD_CTX_free(ctx);

    // Imprimir el hash en formato hexadecimal
    printf("SHA256(\"%s\") = ", cadena);
    for(unsigned int i = 0; i < longitud_hash; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");

    *pt_hash_len = longitud_hash;

    return 0;
}

int8_t faasparser_parse_fc_as_str_json(faas_t* pt_faas, uint8_t* ui8_payload_buff, size_t* payload_len)
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

    uint8_t ui8_payload[2048];
    int fc_len = proto_serialize_fc(pt_faas, ui8_payload, sizeof(ui8_payload));

    char* str_encoded_fc = (char*)malloc(base64_encode_len(strlen(ui8_payload)));

    if (str_encoded_fc == NULL)
    {
        COGNIT_LOG_ERROR("Failed to allocate memory for encoded string");
    }

    root = cJSON_CreateObject();

    if (root == NULL)
    {
        COGNIT_LOG_ERROR("Error creating cJSON object");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    cJSON_AddStringToObject(root, "LANG", "C");

    out_fc_len = base64_encode(str_encoded_fc, ui8_payload, fc_len);
    COGNIT_LOG_DEBUG("str_encoded_fc: %s", str_encoded_fc);
    // COGNIT_LOG_DEBUG("strlen(str_encoded_fc): %ld", strlen(str_encoded_fc));
    cJSON_AddStringToObject(root, "FC", (const char*)str_encoded_fc);
    free(str_encoded_fc);
    
    
    char str_hash[2048];
    unsigned char bin_hash[EVP_MAX_MD_SIZE];
    int hash_len;
    char fc_hex[1024 * 16];
    hash_a_hex(pt_faas->myfunc.fc_code, strlen(pt_faas->myfunc.fc_code), fc_hex);
    calc_hash(fc_hex, bin_hash, &hash_len);
    hash_a_hex(bin_hash, hash_len, str_hash);
    cJSON_AddStringToObject(root, "FC_HASH", str_hash);


    str_faas_json = cJSON_Print(root);

    // Copy the json string to the payload buffer
    strcpy((char*)ui8_payload_buff, str_faas_json);
    *payload_len = strlen(str_faas_json);

    cJSON_Delete(root);
    free(str_faas_json);

    return JSON_ERR_CODE_OK;
}

int8_t faasparser_parse_json_str_as_fc_id_response(const char* json_str, size_t len)
{
    COGNIT_LOG_ERROR("CFC devuelve esto: %s", json_str);
    /*cJSON* root = cJSON_Parse(json_str);

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

    t_exec_response->ret_code = ret_code_item->valueint;

    // Decode base64 res_item
    int out_len = 0;

    t_exec_response->res_payload = (char*)malloc(base64_decode_len(res_item->valuestring));
    if (t_exec_response->res_payload == NULL)
    {
        COGNIT_LOG_ERROR("Failed to allocate memory for t_exec_response->res_payload");
    }
    out_len = base64_decode(t_exec_response->res_payload, res_item->valuestring);

    if (t_exec_response->res_payload == NULL)
    {
        COGNIT_LOG_ERROR("Error decoding base64");
        cJSON_Delete(root);
        return JSON_ERR_CODE_INVALID_JSON;
    }

    t_exec_response->res_payload_len = out_len;
    // TODO: parse err ??

    cJSON_Delete(root);
    */
    return 0;//JSON_ERR_CODE_OK;
}

int8_t faasparser_parse_exec_faas_params_as_str_json(faas_t* pt_faas, uint8_t* ui8_payload_buff, size_t* payload_len)
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
        param = cJSON_CreateObject();
        if (param == NULL)
        {
            COGNIT_LOG_ERROR("Error creating cJSON object");
            cJSON_Delete(root);
            return JSON_ERR_CODE_INVALID_JSON;
        }
        uint8_t str_param[1024 * 16];
        int param_len = proto_serialize_faas_param(pt_faas, i, str_param, sizeof(str_param));
        /*************************/

        str_b64_param = malloc(base64_encode_len(param_len));
        if (str_b64_param == NULL)
        {
            COGNIT_LOG_ERROR("Failed to allocate memory for encoded param");
        }

        i_coded_value_len = base64_encode(str_b64_param, str_param, param_len);

        

        
        cJSON_AddItemToArray(params_array, cJSON_CreateString(str_b64_param));

        free(str_b64_param);
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

    uint8_t i8_ret = proto_deserialize_faas_param(res_payload, out_len, &pt_exec_response->res);

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



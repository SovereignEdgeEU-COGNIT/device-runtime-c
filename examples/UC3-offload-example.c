#include <stdio.h>
#include "cognit_http.h"
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <device_runtime.h>
#include <unistd.h>
#include <cognit_http.h>
#include <cognit_encoding.h>
#include <logger.h>
//#include <ip_utils.h>
#include <mbedtls/base64.h>
#include <mbedtls/sha256.h>

int my_base64_encode_cb(unsigned char str_b64_buff[], size_t buff_len, size_t *base64_len, char str[], int str_len)
{
    return mbedtls_base64_encode(str_b64_buff, buff_len, base64_len, str, str_len);
}

int my_base64_decode_cb(char decoded_buff[], size_t buff_size, size_t* decoded_len, const unsigned char* str, size_t str_len)
{
    return mbedtls_base64_decode(decoded_buff, buff_size, decoded_len, str, str_len);
}

int my_hash_cb(const unsigned char* str, size_t str_len, unsigned char hash[])
{
    return mbedtls_sha256_ret(str, str_len, hash, 0);
}

size_t handle_response_data_cb(void* data_content, size_t size, size_t nmemb, void* user_buffer)
{
    size_t realsize           = size * nmemb;
    http_response_t* response = (http_response_t*)user_buffer;

    if (response->size + realsize >= sizeof(response->ui8_response_data_buffer))
    {
        COGNIT_LOG_ERROR("Response buffer too small");
        return 0;
    }

    memcpy(&(response->ui8_response_data_buffer[response->size]), data_content, realsize);
    response->size += realsize;
    response->ui8_response_data_buffer[response->size] = '\0';

    return realsize;
}

int8_t my_http_send_req_cb(const char* c_buffer, size_t size, http_config_t* config)
{
    CURL* curl;
    CURLcode res = CURLE_OK;
    long http_code             = 0;
    struct curl_slist* headers = NULL;
    memset(&config->t_http_response.ui8_response_data_buffer, 0, sizeof(config->t_http_response.ui8_response_data_buffer));
    config->t_http_response.size = 0;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (curl)
    {
        // Set the request header
        headers = curl_slist_append(headers, "Accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        //headers = curl_slist_append(headers, "charset: utf-8");

        if (config->c_token != NULL)
        {
            char token_header[MAX_TOKEN_LENGTH] = "token: ";
            strcat(token_header, config->c_token);
            headers = curl_slist_append(headers, token_header);
        }

        if (curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers) != CURLE_OK
            // Configure URL and payload
            || curl_easy_setopt(curl, CURLOPT_URL, config->c_url) != CURLE_OK
            // Set the callback function to handle the response data
            || curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&config->t_http_response) != CURLE_OK
            || curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handle_response_data_cb) != CURLE_OK
            || curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, config->ui32_timeout_ms) != CURLE_OK
            || curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L) != CURLE_OK
            || curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L) != CURLE_OK)
        {
            COGNIT_LOG_ERROR("[http_send_req_cb] curl_easy_setopt() failed");
            return -1;
        }

        // Find '[' or ']' in the URL to determine the IP version
        // TODO: fix ip_utils to obtain http://[2001:67c:22b8:1::d]:8000/v1/faas/execute-sync
        // as IP_V6
        if (strchr(config->c_url, '[') != NULL
            && strchr(config->c_url, ']') != NULL)
        {
            if (curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V6) != CURLE_OK)
            {
                COGNIT_LOG_ERROR("[http_send_req_cb] curl_easy_setopt()->IPRESOLVE_V6 failed");
                return -1;
            }
        }

        if (strcmp(config->c_method, HTTP_METHOD_GET) == 0)
        {
            if (curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_USERNAME, config->c_username) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_PASSWORD, config->c_password) != CURLE_OK)
            {
                COGNIT_LOG_ERROR("[http_send_req_cb] curl_easy_setopt()->get() failed");
                return -1;
            }
        }
        else if (strcmp(config->c_method, HTTP_METHOD_POST) == 0)
        {
            if (curl_easy_setopt(curl, CURLOPT_POST, 1L) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST") != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, size) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_POSTFIELDS, c_buffer) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_USERNAME, config->c_username) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_PASSWORD, config->c_password) != CURLE_OK)
            {
                COGNIT_LOG_ERROR("[http_send_req_cb] curl_easy_setopt()->post() failed");
                return -1;
            }
        }
        else if (strcmp(config->c_method, HTTP_METHOD_PUT) == 0)
        {
            if (curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT") != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, size) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_POSTFIELDS, c_buffer) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_USERNAME, config->c_username) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_PASSWORD, config->c_password) != CURLE_OK)
            {
                COGNIT_LOG_ERROR("[http_send_req_cb] curl_easy_setopt()->put() failed");
                return -1;
            }
        }
        else if (strcmp(config->c_method, HTTP_METHOD_DELETE) == 0)
        {
            if (curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE") != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_USERNAME, config->c_username) != CURLE_OK
                || curl_easy_setopt(curl, CURLOPT_PASSWORD, config->c_password) != CURLE_OK)
            {
                COGNIT_LOG_ERROR("[http_send_req_cb] curl_easy_setopt()->post() failed");
                return -1;
            }
        }
        else
        {
            COGNIT_LOG_ERROR("[http_send_req_cb] Invalid HTTP method");
            return -1;
        }

        // Make the request
        res = curl_easy_perform(curl);

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        COGNIT_LOG_INFO("HTTP err code %ld ", http_code);

        // Check errors
        if (res != CURLE_OK)
        {
            http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            COGNIT_LOG_ERROR("curl_easy_perform() failed: %s", curl_easy_strerror(res));
            COGNIT_LOG_ERROR("HTTP err code %ld ", http_code);
        }

        // Clean and close CURL session
        curl_easy_cleanup(curl);
    }

    config->t_http_response.l_http_code = http_code;

    // Clean global curl configuration
    curl_global_cleanup();
    curl_slist_free_all(headers);

    return (res == CURLE_OK) ? 0 : -1;
}

cognit_config_t t_config = {
    .cognit_frontend_endpoint   = "https://cognit-lab-frontend.sovereignedge.eu",
    .cognit_frontend_usr        = "", // Put your username here.
    .cognit_frontend_pwd        = "", // Put your password here.
};

// Set your own App requirements.
scheduling_t app_reqs = {
    .flavour                     = "FaaS_generic_V2", // Put a Flavour that your username is allowed to use.
    .max_latency                 = 100,		      // Max latency required in miliseconds.
    .max_function_execution_time = 3.5,		      // Max execution time required in seconds.
    .min_renewable               = 85,		      // Minimal renewable energy resources required in percentage.
    .geolocation                 = "IKERLAN ARRASATE/MONDRAGON 20500"
};

// Set your new App requirements.
scheduling_t new_reqs = {
    .flavour                     = "FaaS_generic_V2", // Put a Flavour that your username is allowed to use.
    .max_latency                 = 80,		      // Max latency required in miliseconds.
    .max_function_execution_time = 8.5,               // Max execution time required in seconds.
    .min_renewable               = 50,                // Minimal renewable energy resources required in percentage.
    .geolocation                 = "IKERLAN ARRASATE/MONDRAGON 20500"
};

char *mock_decision_algo_py =
"def mock_decision_algo(\n"
"    timestamp: float,\n"
"    trained_model: bytes,\n"
"    home_model_parameters: str,\n"
"    storage_parameters: str,\n"
"    ev_battery_parameters: str,\n"
"    heating_parameters_per_room: str,\n"
"    energy_pv_produced_pred: float,\n"
"    uncontrolled_energy_consumption_pred: float,\n"
"    temp_outside_pred: float,\n"
"    cycle_timedelta_s: int):\n"
"    \"\"\"\n"
"    Convierte los parámetros JSON a dicts, calcula las decisiones y devuelve\n"
"    tres strings formateados en JSON con conf_temp_per_room, storage_params y ev_params.\n"
"    \"\"\"\n"
"    # 1. Parsear JSON a dicts/listas\n"
"    home_model_parameters_dict = json.loads(home_model_parameters)\n"
"    storage_parameters_dict = json.loads(storage_parameters)\n"
"    ev_battery_parameters_dict = json.loads(ev_battery_parameters)\n"
"    heating_parameters_list = json.loads(heating_parameters_per_room)\n"
"\n"
"    # Opcional: imprimir valores para debug\n"
"    print(\"timestamp:\", timestamp)\n"
"    print(\"trained_model (bytes):\", trained_model)\n"
"    print(\"home_model_parameters:\", home_model_parameters_dict)\n"
"    print(\"storage_parameters:\", storage_parameters_dict)\n"
"    print(\"ev_battery_parameters:\", ev_battery_parameters_dict)\n"
"    print(\"heating_parameters_per_room:\", heating_parameters_list)\n"
"    print(\"energy_pv_produced_pred:\", energy_pv_produced_pred)\n"
"    print(\"uncontrolled_energy_consumption_pred:\", uncontrolled_energy_consumption_pred)\n"
"    print(\"temp_outside_pred:\", temp_outside_pred)\n"
"    print(\"cycle_timedelta_s:\", cycle_timedelta_s)\n"
"\n"
"    # 2.1. conf_temp_per_room: usar preferred_temp tal cual\n"
"    conf_temp_per_room = {}\n"
"    for idx, room_params in enumerate(heating_parameters_list):\n"
"        room_name = f\"room_{idx}\"\n"
"        conf_temp_per_room[room_name] = room_params[\"preferred_temp\"]\n"
"\n"
"    # 2.2. storage_params: intentar cargar un 10% más si cabe\n"
"    max_cap = storage_parameters_dict[\"max_capacity\"]\n"
"    curr_charge = storage_parameters_dict[\"curr_charge_level\"]\n"
"    target = min(curr_charge + 0.10 * max_cap, max_cap)\n"
"    storage_params = {\n"
"        \"charge_rate\": storage_parameters_dict[\"nominal_power\"],\n"
"        \"target_charge_level\": target\n"
"    }\n"
"\n"
"    # 2.3. ev_params: si está disponible, cargar al 10% más\n"
"    ev_params = {}\n"
"    if ev_battery_parameters_dict[\"is_available\"]:\n"
"        curr_ev = ev_battery_parameters_dict[\"curr_charge_level\"]\n"
"        max_ev = ev_battery_parameters_dict[\"max_capacity\"]\n"
"        target_ev = min(curr_ev + 0.10 * max_ev, max_ev)\n"
"        ev_params = {\n"
"            \"charge_power\": ev_battery_parameters_dict[\"nominal_power\"],\n"
"            \"target_soc\": target_ev,\n"
"            \"departure_time\": ev_battery_parameters_dict[\"time_until_charged\"]\n"
"        }\n"
"    else:\n"
"        curr_ev = ev_battery_parameters_dict[\"curr_charge_level\"]\n"
"        ev_params = {\n"
"            \"charge_power\": 0.0,\n"
"            \"target_soc\": curr_ev,\n"
"            \"departure_time\": 0\n"
"        }\n"
"\n"
"    # 3. Convertir los dicts a strings JSON\n"
"    conf_temp_per_room_str = json.dumps(conf_temp_per_room)\n"
"    storage_params_str = json.dumps(storage_params)\n"
"    ev_params_str = json.dumps(ev_params)\n"
"\n"
"    return conf_temp_per_room_str, storage_params_str, ev_params_str\n";

#define NUM_ROOMS 2

int main(int argc, char const* argv[])
{
    /* timestamp */
    double timestamp = 1622548800.0;
    
    /* trained_model (bytes) */
    uint8_t trained_model[] = { 0xDE, 0xAD, 0xBE, 0xEF };
    size_t trained_model_len = sizeof(trained_model);

    /* home_model_parameters en JSON */
    const char *home_model_json =
        "{\"heat_capacity\":1200.0,"
        "\"heating_delta_temperature\":2.5,"
        "\"heating_coefficient\":0.8,"
        "\"heat_loss_coefficient\":0.05,"
        "\"min_temp_setting\":18.0,"
        "\"max_temp_setting\":24.0}";

    /* storage_parameters en JSON */
    const char *storage_json =
        "{\"max_capacity\":10.0,"
        "\"nominal_power\":5.0,"
        "\"efficiency\":0.9,"
        "\"curr_charge_level\":7.5}";

    /* ev_battery_parameters en JSON */
    const char *ev_battery_json =
        "{\"max_capacity\":60.0,"
        "\"is_available\":true,"
        "\"driving_charge_level\":80.0,"
        "\"nominal_power\":7.0,"
        "\"efficiency\":0.95,"
        "\"curr_charge_level\":40.0,"
        "\"time_until_charged\":3600}";

    /* heating_parameters_per_room: lista de JSONs (2 habitaciones) */
    const char *heating_params_json = 
    "["
        "{\"curr_temp\":20.5,\"preferred_temp\":21.0},"
        "{\"curr_temp\":19.0,\"preferred_temp\":20.0}"
    "]";

    /* Predicciones y ciclo */
    double energy_pv_produced_pred = 5.25;
    double uncontrolled_energy_consumption_pred = 2.75;
    double temp_outside_pred = 15.0;
    int cycle_timedelta_s = 3600;
    
    device_runtime_t t_my_device_runtime;
    faas_t t_faas;
    char* exec_response[3];
    e_status_code_t ret;
    
    device_runtime_init(&t_my_device_runtime, t_config, app_reqs, &t_faas);

    addFC(&t_faas, mock_decision_algo_py);
    
    addDOUBLEVar(&t_faas, timestamp);
    addBYTESParam(&t_faas, trained_model, 4/*length*/);
    addSTRINGParam(&t_faas, home_model_json);
    addSTRINGParam(&t_faas, storage_json);
    addSTRINGParam(&t_faas, ev_battery_json);
    addSTRINGParam(&t_faas, heating_params_json);
    addDOUBLEVar(&t_faas, energy_pv_produced_pred);
    addDOUBLEVar(&t_faas, uncontrolled_energy_consumption_pred);
    addDOUBLEVar(&t_faas, temp_outside_pred);
    addINT32Var(&t_faas, cycle_timedelta_s);
    
    ret = device_runtime_call(&t_my_device_runtime, &t_faas, new_reqs, (void*)exec_response);

    if (ret == E_ST_CODE_SUCCESS)
    {
        COGNIT_LOG_INFO("Result: %s", exec_response[0]);
        COGNIT_LOG_INFO("Result: %s", exec_response[1]);
        COGNIT_LOG_INFO("Result: %s", exec_response[2]);

        //Free response pointers
        for (int i = 0; i < 3; i++) {
            free(exec_response[i]);
        }
    }
    else
    {
        COGNIT_LOG_ERROR("Error offloading function");
    }
    return 0;
}



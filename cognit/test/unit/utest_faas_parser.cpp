#include <utest_faas_parser.h>
extern "C" {
#include <faas_parser.h>
#include <base64.h>
}

FUNC_TO_STR(
    suma,
    void suma(int a, int b, float* c) {
        *c = a + b;
    })

TEST_F(ITestFaasParser, TestParseExecParamsAsJson)
{
    exec_faas_params_t exec_params = { 0 };
    size_t base64_fc_len           = 0;
    uint8_t ui8_json_buffer[1024]  = { 0 };
    size_t ui8_json_buffer_len     = 0;
    int8_t i8_ret                  = 0;

    const char* includes = INCLUDE_HEADERS(#include<stdio.h> \n);
    size_t total_fc_len  = strlen(suma_str) + strlen(includes) + 1;

    char* c_raw_fc = (char*)malloc(total_fc_len);
    strcpy(c_raw_fc, includes);
    strcat(c_raw_fc, suma_str);

    printf("C function: %s\n", c_raw_fc);

    strcpy(exec_params.lang, "C");
    exec_params.fc = c_raw_fc;

    // Asegúrate de que params es un array de al menos 3 elementos
    exec_params.params       = (param_t*)malloc(sizeof(param_t) * 3);
    exec_params.params_count = 3;

    // Primer parámetro de entrada
    exec_params.params[0].type     = "int";
    exec_params.params[0].var_name = "a";
    exec_params.params[0].value    = "3";
    strncpy(exec_params.params[0].mode, "IN", sizeof(exec_params.params[0].mode));

    // Segundo parámetro de entrada
    exec_params.params[1].type     = "int";
    exec_params.params[1].var_name = "b";
    exec_params.params[1].value    = "4";
    strncpy(exec_params.params[1].mode, "IN", sizeof(exec_params.params[0].mode));

    // Parámetro de salida
    exec_params.params[2].type     = "float";
    exec_params.params[2].var_name = "c";
    exec_params.params[2].value    = NULL;
    strncpy(exec_params.params[2].mode, "OUT", sizeof(exec_params.params[0].mode));

    i8_ret = parse_exec_faas_params_as_str_json(&exec_params, ui8_json_buffer, &ui8_json_buffer_len);

    printf("JSON: %s\n", (char*)ui8_json_buffer);

    free(c_raw_fc);
    free(exec_params.params);
}

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
    const char* c_test_json        = "{\n\t\"lang\":\t\"C\",\n\t\"fc\":\t\"I2luY2x1ZGU8c3RkaW8uaD4gCnZvaWQgc3VtYShpbnQgYSwgaW50IGIsIGZsb2F0KiBjKSB7ICpjID0gYSArIGI7IH0=\",\n\t\"params\":\t[\"ewoJInR5cGUiOgkiaW50IiwKCSJ2YXJfbmFtZSI6CSJhIiwKCSJ2YWx1ZSI6CSJNdz09IiwKCSJtb2RlIjoJIklOIgp9\", \"ewoJInR5cGUiOgkiaW50IiwKCSJ2YXJfbmFtZSI6CSJiIiwKCSJ2YWx1ZSI6CSJOQT09IiwKCSJtb2RlIjoJIklOIgp9\", \"ewoJInR5cGUiOgkiZmxvYXQiLAoJInZhcl9uYW1lIjoJImMiLAoJInZhbHVlIjoJIk5VTEwiLAoJIm1vZGUiOgkiT1VUIgp9\"]\n}";
    exec_faas_params_t exec_params = { 0 };
    size_t base64_fc_len           = 0;
    uint8_t ui8_json_buffer[1024]  = { 0 };
    size_t size_json_buff          = 0;
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

    i8_ret = parse_exec_faas_params_as_str_json(&exec_params, ui8_json_buffer, &size_json_buff);

    printf("JSON: %s\n", (char*)ui8_json_buffer);
    printf("JSON len: %ld\n", size_json_buff);

    EXPECT_STREQ(c_test_json, (char*)ui8_json_buffer);

    free(c_raw_fc);
    free(exec_params.params);
}

// Test to parse json as exec_response_t
TEST_F(ITestFaasParser, TestParseJsonAsExecResponse)
{
    const char* c_test_json = "{\"ret_code\":0,\"res\":\"Ny4w\",\"err\":null}";
    int8_t i8_ret           = 0;
    exec_response_t test_exec_response;
    i8_ret = parse_json_str_as_exec_response(c_test_json, &test_exec_response);

    EXPECT_EQ(test_exec_response.ret_code, 0);
    EXPECT_STREQ(test_exec_response.res_payload, "7.0");
}

TEST_F(ITestFaasParser, TestParseJsonAsAsyncExecResponseNull)
{
    const char* c_test_json = "{\"status\":\"WORKING\",\"res\":null,\"exec_id\":{\"faas_task_uuid\":\"58035b11-aa14-11ee-a240-02008ac90074\"}}";
    int8_t i8_ret           = 0;
    async_exec_response_t test_async_exec_response;
    i8_ret = parse_json_str_as_async_exec_response(c_test_json, &test_async_exec_response);

    printf("Status: %s\n", test_async_exec_response.status);
    printf("Exec ID: %s\n", test_async_exec_response.exec_id.faas_task_uuid);

    EXPECT_STREQ(test_async_exec_response.status, "WORKING");
    if (test_async_exec_response.res == NULL)
    {
        printf("Res is NULL\n");
        EXPECT_EQ(0, 0);
    }
    else
    {
        printf("Res is NOT NULL\n");
        EXPECT_EQ(0, 1);
    }
    EXPECT_STREQ(test_async_exec_response.exec_id.faas_task_uuid, "58035b11-aa14-11ee-a240-02008ac90074");
}

TEST_F(ITestFaasParser, TestParseJsonAsAsyncExecResponseComplete)
{
    const char* c_test_json = "{\"status\":\"READY\",\"res\":{\"ret_code\":0,\"res\":\"MTA=\",\"err\":null},\"exec_id\":{\"faas_task_uuid\":\"58035b11-aa14-11ee-a240-02008ac90074\"}}";
    int8_t i8_ret           = 0;
    async_exec_response_t test_async_exec_response;
    i8_ret = parse_json_str_as_async_exec_response(c_test_json, &test_async_exec_response);

    printf("Status: %s\n", test_async_exec_response.status);
    printf("Exec ID: %s\n", test_async_exec_response.exec_id.faas_task_uuid);
    printf("Ret code: %d\n", test_async_exec_response.res->ret_code);
    printf("Res: %s\n", test_async_exec_response.res->res_payload);

    EXPECT_STREQ(test_async_exec_response.status, "READY");
    EXPECT_EQ(test_async_exec_response.res->ret_code, 0);
    EXPECT_STREQ(test_async_exec_response.res->res_payload, "10");
    EXPECT_STREQ(test_async_exec_response.exec_id.faas_task_uuid, "58035b11-aa14-11ee-a240-02008ac90074");
}
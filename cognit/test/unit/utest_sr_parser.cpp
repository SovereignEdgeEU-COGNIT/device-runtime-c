#include <utest_sr_parser.h>
extern "C" {
#include <sr_parser.h>
#include <base64.h>
#include <logger.h>
}

TEST_F(UTestSRParser, TestParseServerlessRuntimeAsJson)
{
    const char* c_test_json        = "{\n\t\"name\":\t\"test_serverless_runtime\",\n"
    "\t\"id\":\t2,\n"
    "\t\"faas\":\t{\n"
        "\t\t\"cpu\":\t1,\n"
        "\t\t\"memory\":\t768,\n"
        "\t\t\"disk_size\":\t3072,\n"
        "\t\t\"flavour\":\t\"nature\"\n"
        // "\t\t\"endpoint\":\t\"\",\n"
        // "\t\t\"state\":\t\"\",\n"
        // "\t\t\"vm_id\":\t\"\"\n"
        "\t}\n}";
    serverless_runtime_t test_serverless_runtime = { 0 };
    uint8_t ui8_json_buffer[1024]  = { 0 };
    size_t size_json_buff          = 0;
    int8_t i8_ret                  = 0;

    // Initialize the values in the structure
    strcpy(test_serverless_runtime.c_name, "test_serverless_runtime");

    test_serverless_runtime.ui32_id = 2;

    test_serverless_runtime.faas_config.ui8_cpu        = 1;
    test_serverless_runtime.faas_config.ui32_memory    = 768;
    test_serverless_runtime.faas_config.ui32_disk_size = 3072;
    strcpy(test_serverless_runtime.faas_config.c_flavour, "nature");
    strcpy(test_serverless_runtime.faas_config.c_endpoint, "");
    strcpy(test_serverless_runtime.faas_config.c_state, "");
    strcpy(test_serverless_runtime.faas_config.c_vm_id, "");

    COGNIT_LOG_DEBUG("Serverless runtime created");

    i8_ret = srparser_parse_serverless_runtime_as_str_json(&test_serverless_runtime, ui8_json_buffer, &size_json_buff);

    COGNIT_LOG_DEBUG("JSON: %s", (char*)ui8_json_buffer);
    COGNIT_LOG_DEBUG("JSON len: %ld", size_json_buff);

    EXPECT_STREQ(c_test_json, (char*)ui8_json_buffer);
}

// Test to parse json as serverless_runtime_t
TEST_F(UTestSRParser, TestParseJsonAsServerlessRuntime)
{
    const char* c_test_json        = "{\n\t\"name\":\t\"test_serverless_runtime\",\n"
    "\t\"id\":\t2,\n"
    "\t\"faas\":\t{\n"
        "\t\t\"cpu\":\t1,\n"
        "\t\t\"memory\":\t768,\n"
        "\t\t\"disk_size\":\t3072,\n"
        "\t\t\"flavour\":\t\"nature\",\n"
        "\t\t\"endpoint\":\t\"localhost:8080\",\n"
        "\t\t\"state\":\t\"pending\",\n"
        "\t\t\"vm_id\":\t\"id\"\n"
        "\t}\n}";
        serverless_runtime_t test_serverless_runtime = { 0 };
        int8_t i8_ret                  = 0;
    
    COGNIT_LOG_DEBUG("JSON: %s", c_test_json);

    i8_ret = srparser_parse_json_str_as_serverless_runtime(c_test_json, &test_serverless_runtime);

    EXPECT_STREQ(test_serverless_runtime.c_name, "test_serverless_runtime");
    EXPECT_EQ(test_serverless_runtime.ui32_id, 2);
    EXPECT_EQ(test_serverless_runtime.faas_config.ui8_cpu, 1);
    EXPECT_EQ(test_serverless_runtime.faas_config.ui32_memory, 768);
    EXPECT_EQ(test_serverless_runtime.faas_config.ui32_disk_size, 3072);
    EXPECT_STREQ(test_serverless_runtime.faas_config.c_flavour, "nature");
    EXPECT_STREQ(test_serverless_runtime.faas_config.c_endpoint, "localhost:8080");
    EXPECT_STREQ(test_serverless_runtime.faas_config.c_state, "pending");
    EXPECT_STREQ(test_serverless_runtime.faas_config.c_vm_id, "id");
}

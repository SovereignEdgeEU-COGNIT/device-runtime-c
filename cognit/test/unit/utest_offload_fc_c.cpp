#include <utest_offload_fc_c.h>
extern "C" {

#include <offload_fc_c.h>
#include <faas_parser.h>
#include <logger.h>
}

FUNC_TO_STR(
    suma_fc,
    void suma_fc(int a, int b, float* c) {
        *c = a + b;
    })

TEST_F(UTestOffload_fc_c, TestOffloadFc)
{
    exec_faas_params_t exec_params = { 0 };

    const char* includes = INCLUDE_HEADERS(#include<stdio.h> \n);
    offload_fc_c_create(&exec_params, includes, suma_fc_str);

    COGNIT_LOG_DEBUG("C function: %s", exec_params.fc);

    // Param 1
    offload_fc_c_add_param(&exec_params, "a", "IN");
    offload_fc_c_set_param(&exec_params, "int", "3");
    // Param 2
    offload_fc_c_add_param(&exec_params, "b", "IN");
    offload_fc_c_set_param(&exec_params, "int", "4");
    // Param 3
    offload_fc_c_add_param(&exec_params, "c", "OUT");
    offload_fc_c_set_param(&exec_params, "float", NULL);

    for (int i = 0; i < exec_params.params_count; i++)
    {
        if (exec_params.params[i].value == NULL)
        {
            COGNIT_LOG_INFO("Param %d: %s %s %s %s", i, exec_params.params[i].mode, exec_params.params[i].type, exec_params.params[i].var_name, "NULL");
            continue;
        }
        else
        {
            COGNIT_LOG_INFO("Param %d: %s %s %s %s", i, exec_params.params[i].mode, exec_params.params[i].type, exec_params.params[i].var_name, exec_params.params[i].value);
        }
    }

    EXPECT_STREQ(exec_params.lang, "C");

    EXPECT_STREQ(exec_params.params[0].mode, "IN");
    EXPECT_STREQ(exec_params.params[0].type, "int");
    EXPECT_STREQ(exec_params.params[0].var_name, "a");
    EXPECT_STREQ(exec_params.params[0].value, "3");

    EXPECT_STREQ(exec_params.params[1].mode, "IN");
    EXPECT_STREQ(exec_params.params[1].type, "int");
    EXPECT_STREQ(exec_params.params[1].var_name, "b");
    EXPECT_STREQ(exec_params.params[1].value, "4");

    EXPECT_STREQ(exec_params.params[2].mode, "OUT");
    EXPECT_STREQ(exec_params.params[2].type, "float");
    EXPECT_STREQ(exec_params.params[2].var_name, "c");
    if (exec_params.params[2].value == NULL)
    {
        EXPECT_EQ(0, 0);
    }

    offload_fc_c_destroy(&exec_params);
}

#include <utest_base64.h>
extern "C" {
#include <base64.h>
#include <logger.h>
}

TEST_F(UTestBase64, TestEncode)
{
    const char* data              = "¡Hola, 你好, مرحبًا, Γεια σας, こんにちは, שלום, Привет!";
    const char* encoded_data_test = "wqFIb2xhLCDkvaDlpb0sINmF2LHYrdio2YvYpywgzpPOtc65zrEgz4POsc+CLCDjgZPjgpPjgavjgaHjga8sINep15zXldedLCDQn9GA0LjQstC10YIh";
    int input_size                = strlen(data);
    int encoded_len               = base64_encode_len(input_size);
    int coded_len                 = 0;

    char* encoded_data = (char*)malloc(encoded_len);
    if (encoded_data == NULL)
    {
        COGNIT_LOG_ERROR("Failed to allocate memory for encoded string");
    }
    coded_len = base64_encode(encoded_data, data, input_size);
    COGNIT_LOG_DEBUG("Encoded data: %s", encoded_data);
    COGNIT_LOG_DEBUG("Encoded data size: %d", encoded_len);
    COGNIT_LOG_DEBUG("Encoded data size test: %ld", strlen(encoded_data_test));

    EXPECT_STREQ(encoded_data_test, encoded_data);
    EXPECT_EQ(encoded_len, coded_len);
    free(encoded_data);
}

TEST_F(UTestBase64, TestDecode)
{
    const char* decoded_data_test = "¡Hola, 你好, مرحبًا, Γεια σας, こんにちは, שלום, Привет!";
    const char* data              = "wqFIb2xhLCDkvaDlpb0sINmF2LHYrdio2YvYpywgzpPOtc65zrEgz4POsc+CLCDjgZPjgpPjgavjgaHjga8sINep15zXldedLCDQn9GA0LjQstC10YIh";
    int input_size                = strlen(data);

    char* decoded = (char*)malloc(input_size + 1);
    if (decoded == NULL)
    {
        COGNIT_LOG_ERROR("Failed to allocate memory for decoded string");
    }
    int decoded_len = base64_decode(decoded, data);
    COGNIT_LOG_DEBUG("Encoded data: %s", decoded);
    COGNIT_LOG_DEBUG("Encoded data size: %d", decoded_len);
    COGNIT_LOG_DEBUG("Encoded data size test: %ld", strlen(data));
    free(decoded);
}

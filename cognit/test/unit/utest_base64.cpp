#include <utest_base64.h>
#include <curl/curl.h>
extern "C" {
#include <base64.h>
}


TEST_F(ITestBase64, TestEncode)
{
    const char * data = "¡Hola, 你好, مرحبًا, Γεια σας, こんにちは, שלום, Привет!";
    const char* encoded_data_test = "wqFIb2xhLCDkvaDlpb0sINmF2LHYrdio2YvYpywgzpPOtc65zrEgz4POsc+CLCDjgZPjgpPjgavjgaHjga8sINep15zXldedLCDQn9GA0LjQstC10YIh";
    long input_size = strlen(data);
    size_t output_size = 0;
    char * encoded_data = base64_encode(data, input_size, &output_size);
    printf("Encoded data: %s\n", encoded_data);
    printf("Encoded data size: %ld\n", output_size);
    printf("Encoded data size test: %ld\n", strlen(encoded_data_test));

    EXPECT_STREQ(encoded_data_test, encoded_data);
}

TEST_F(ITestBase64, TestDecode)
{
    const char * decoded_data_test = "¡Hola, 你好, مرحبًا, Γεια σας, こんにちは, שלום, Привет!";
    const char* data = "wqFIb2xhLCDkvaDlpb0sINmF2LHYrdio2YvYpywgzpPOtc65zrEgz4POsc+CLCDjgZPjgpPjgavjgaHjga8sINep15zXldedLCDQn9GA0LjQstC10YIh";
    size_t input_size = strlen(data);
    size_t output_size = 0;
    char * decoded_data = base64_decode( data, input_size, &output_size);
    printf("Encoded data: %s\n", decoded_data);
    printf("Encoded data size: %ld\n", output_size);
    printf("Encoded data size test: %ld\n", strlen(data));

    EXPECT_STREQ(decoded_data_test, (const char*)decoded_data);
}

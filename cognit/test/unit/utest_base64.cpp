#include <utest_base64.h>
extern "C" {
#include <base64.h>
}

TEST_F(UTestBase64, TestEncode)
{
    const char* data              = "¡Hola, 你好, مرحبًا, Γεια σας, こんにちは, שלום, Привет!";
    const char* encoded_data_test = "wqFIb2xhLCDkvaDlpb0sINmF2LHYrdio2YvYpywgzpPOtc65zrEgz4POsc+CLCDjgZPjgpPjgavjgaHjga8sINep15zXldedLCDQn9GA0LjQstC10YIh";
    int input_size                = strlen(data);
    int encoded_len    = base64_encode_len(input_size);
    int coded_len = 0;

    char* encoded_data = (char*)malloc(encoded_len);
    if (encoded_data == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for encoded string\n");
    }
    coded_len = base64_encode(encoded_data, data, input_size);
    printf("Encoded data: %s\n", encoded_data);
    printf("Encoded data size: %d\n", encoded_len);
    printf("Encoded data size test: %ld\n", strlen(encoded_data_test));

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
        fprintf(stderr, "Failed to allocate memory for decoded string\n");
    }
    int decoded_len = base64_decode(decoded, data);
    printf("Encoded data: %s\n", decoded);
    printf("Encoded data size: %d\n", decoded_len);
    printf("Encoded data size test: %ld\n", strlen(data));
    free(decoded);
}

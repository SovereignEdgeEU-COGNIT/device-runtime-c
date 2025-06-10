#include "pb_parser.h"
#include "logger.h"

#define MAX_BYTE_PARAMS 8 // Límite máximo de parámetros de tipo bytes

void printParams(faas_t* pt_faas)
{
    COGNIT_LOG_DEBUG("FaaS Request Params:");
    for (size_t i = 0; i < pt_faas->params_count; i++)
    {
        MyParam* param = &pt_faas->params[i];
        COGNIT_LOG_DEBUG("  Param[%zu]:", i);
        
        switch (param->which_param)
        {
            case MyParam_my_string_tag:
                COGNIT_LOG_DEBUG("    STRING = %s", param->param.my_string);
                break;

            case MyParam_my_int32_tag:
                for (size_t j = 0; j < param->param.my_int32.values_count; j++)
                {
                    COGNIT_LOG_DEBUG("    INT32[%zu] = %d", j, param->param.my_int32.values[j]);
                }
                break;

            case MyParam_my_int64_tag:
                for (size_t j = 0; j < param->param.my_int64.values_count; j++)
                {
                    COGNIT_LOG_DEBUG("    INT64[%zu] = %ld", j, param->param.my_int64.values[j]);
                }
                break;

            case MyParam_my_float_tag:
                for (size_t j = 0; j < param->param.my_float.values_count; j++)
                {
                    COGNIT_LOG_DEBUG("    FLOAT[%zu] = %f", j, param->param.my_float.values[j]);
                }
                break;

            case MyParam_my_double_tag:
                for (size_t j = 0; j < param->param.my_double.values_count; j++)
                {
                    COGNIT_LOG_DEBUG("    DOUBLE[%zu] = %lf", j, param->param.my_double.values[j]);
                }
                break;

            case MyParam_my_uint32_tag:
                for (size_t j = 0; j < param->param.my_uint32.values_count; j++)
                {
                    COGNIT_LOG_DEBUG("    UINT32[%zu] = %u", j, param->param.my_uint32.values[j]);
                }
                break;

            case MyParam_my_uint64_tag:
                for (size_t j = 0; j < param->param.my_uint64.values_count; j++)
                {
                    COGNIT_LOG_DEBUG("    UINT64[%zu] = %lu", j, param->param.my_uint64.values[j]);
                }
                break;

            case MyParam_my_sint32_tag:
                for (size_t j = 0; j < param->param.my_sint32.values_count; j++)
                {
                    COGNIT_LOG_DEBUG("    SINT32[%zu] = %d", j, param->param.my_sint32.values[j]);
                }
                break;

            case MyParam_my_sint64_tag:
                for (size_t j = 0; j < param->param.my_sint64.values_count; j++)
                {
                    COGNIT_LOG_DEBUG("    SINT64[%zu] = %ld", j, param->param.my_sint64.values[j]);
                }
                break;

            case MyParam_my_fixed32_tag:
                for (size_t j = 0; j < param->param.my_fixed32.values_count; j++)
                {
                    COGNIT_LOG_DEBUG("    FIXED32[%zu] = %u", j, param->param.my_fixed32.values[j]);
                }
                break;

            case MyParam_my_fixed64_tag:
                for (size_t j = 0; j < param->param.my_fixed64.values_count; j++)
                {
                    COGNIT_LOG_DEBUG("    FIXED64[%zu] = %lu", j, param->param.my_fixed64.values[j]);
                }
                break;

            case MyParam_my_sfixed32_tag:
                for (size_t j = 0; j < param->param.my_sfixed32.values_count; j++)
                {
                    COGNIT_LOG_DEBUG("    SFIXED32[%zu] = %d", j, param->param.my_sfixed32.values[j]);
                }
                break;

            case MyParam_my_sfixed64_tag:
                for (size_t j = 0; j < param->param.my_sfixed64.values_count; j++)
                {
                    COGNIT_LOG_DEBUG("    SFIXED64[%zu] = %ld", j, param->param.my_sfixed64.values[j]);
                }
                break;

            default:
                COGNIT_LOG_DEBUG("    Unknown or unsupported type.");
                break;
        }
    }
}

typedef struct { const uint8_t *data; size_t len; } bytes_ctx_t;

static bool encode_bytes(pb_ostream_t *stream,
                         const pb_field_iter_t *field,
                         void *const *arg)
{
    const bytes_ctx_t *ctx = (const bytes_ctx_t *)(*arg);
    if (!pb_encode_tag_for_field(stream, field)) return false;
    return pb_encode_string(stream, ctx->data, ctx->len);
}


void addBYTESParam(faas_t* pt_faas, uint8_t* bytes, size_t len)
{
    static bytes_ctx_t ctx_pool[MAX_BYTE_PARAMS];
    bytes_ctx_t *ctx = &ctx_pool[pt_faas->params_count];
    ctx->data = bytes;
    ctx->len  = len;

    MyParam p = MyParam_init_zero;
    p.which_param          = MyParam_my_bytes_tag;
    p.param.my_bytes.arg   = ctx;
    p.param.my_bytes.funcs.encode = encode_bytes;

    pt_faas->params[pt_faas->params_count++] = p;
}

void addSTRINGParam(faas_t* pt_faas, const char* string)
{
    MyParam param     = MyParam_init_zero;
    param.which_param = MyParam_my_string_tag;
    int i             = 0;
    
    do
    {
        param.param.my_string[i] = string[i];
        i++;
    }while (string[i] != '\0');

    pt_faas->params[pt_faas->params_count++] = param;
}

ADD_ARRAY_FC(float, FLOAT, float)
ADD_ARRAY_FC(double, DOUBLE, double)
ADD_ARRAY_FC(int64, INT64, int64_t)
ADD_ARRAY_FC(int32, INT32, int32_t)
ADD_ARRAY_FC(uint32, UINT32, uint32_t)
ADD_ARRAY_FC(uint64, UINT64, uint64_t)
ADD_ARRAY_FC(sint32, SINT32, int32_t)
ADD_ARRAY_FC(sint64, SINT64, int64_t)
ADD_ARRAY_FC(fixed32, FIXED32, uint32_t)
ADD_ARRAY_FC(fixed64, FIXED64, uint64_t)
ADD_ARRAY_FC(sfixed32, SFIXED32, int32_t)
ADD_ARRAY_FC(sfixed64, SFIXED64, int64_t)
//ADD_ARRAY_FC(bool, BOOL, protobuf_c_boolean)

ADD_VAR_FC(float, FLOAT, float)
ADD_VAR_FC(double, DOUBLE, double)
ADD_VAR_FC(int64, INT64, int64_t)
ADD_VAR_FC(int32, INT32, int32_t)
ADD_VAR_FC(uint32, UINT32, uint32_t)
ADD_VAR_FC(uint64, UINT64, uint64_t)
ADD_VAR_FC(sint32, SINT32, int32_t)
ADD_VAR_FC(sint64, SINT64, int64_t)
ADD_VAR_FC(fixed32, FIXED32, uint32_t)
ADD_VAR_FC(fixed64, FIXED64, uint64_t)
ADD_VAR_FC(sfixed32, SFIXED32, int32_t)
ADD_VAR_FC(sfixed64, SFIXED64, int64_t)
//ADD_VAR_FC(bool, BOOL, protobuf_c_boolean)

static void parse_response(FaasResponse response, void** result, size_t len)
{
    for(int i = 0; i < len; i++)
    {
        MyParam my_param = response.my_faas_response[i];
        
        switch (my_param.which_param)
        {
            case MyParam_my_float_tag:
            {
                size_t count = my_param.param.my_float.values_count;
                float *pt = malloc(count * sizeof(float));
                memcpy(pt,
                       my_param.param.my_float.values,
                       count * sizeof(float));
                result[i] = pt;
                break;
            }
            case MyParam_my_double_tag:
            {
                size_t count = my_param.param.my_double.values_count;
                double *pt = malloc(count * sizeof(double));
                memcpy(pt,
                       my_param.param.my_double.values,
                       count * sizeof(double));
                result[i] = pt;
                break;
            }
            case MyParam_my_int32_tag:
            {
                size_t count = my_param.param.my_int32.values_count;
                int32_t *pt = malloc(count * sizeof(int32_t));
                memcpy(pt,
                       my_param.param.my_int32.values,
                       count * sizeof(int32_t));
                result[i] = pt;
                break;
            }
            case MyParam_my_int64_tag:
            {
                size_t count = my_param.param.my_int64.values_count;
                int64_t *pt = malloc(count * sizeof(int64_t));
                memcpy(pt,
                       my_param.param.my_int64.values,
                       count * sizeof(int64_t));
                result[i] = pt;
                break;
            }
            case MyParam_my_uint32_tag:
            {
                size_t count = my_param.param.my_uint32.values_count;
                uint32_t *pt = malloc(count * sizeof(uint32_t));
                memcpy(pt,
                       my_param.param.my_uint32.values,
                       count * sizeof(uint32_t));
                result[i] = pt;
                break;
            }
            case MyParam_my_uint64_tag:
            {
                size_t count = my_param.param.my_uint64.values_count;
                uint64_t *pt = malloc(count * sizeof(uint64_t));
                memcpy(pt,
                       my_param.param.my_uint64.values,
                       count * sizeof(uint64_t));
                result[i] = pt;
                break;
            }
            case MyParam_my_sint32_tag:
            {
                size_t count = my_param.param.my_sint32.values_count;
                int32_t *pt = malloc(count * sizeof(int32_t));
                memcpy(pt,
                       my_param.param.my_sint32.values,
                       count * sizeof(int32_t));
                result[i] = pt;
                break;
            }
            case MyParam_my_sint64_tag:
            {
                size_t count = my_param.param.my_sint64.values_count;
                int64_t *pt = malloc(count * sizeof(int64_t));
                memcpy(pt,
                       my_param.param.my_sint64.values,
                       count * sizeof(int64_t));
                result[i] = pt;
                break;
            }
            case MyParam_my_fixed32_tag:
            {
                size_t count = my_param.param.my_fixed32.values_count;
                uint32_t *pt = malloc(count * sizeof(uint32_t));
                memcpy(pt,
                       my_param.param.my_fixed32.values,
                       count * sizeof(uint32_t));
                result[i] = pt;
                break;
            }
            case MyParam_my_fixed64_tag:
            {
                size_t count = my_param.param.my_fixed64.values_count;
                uint64_t *pt = malloc(count * sizeof(uint64_t));
                memcpy(pt,
                       my_param.param.my_fixed64.values,
                       count * sizeof(uint64_t));
                result[i] = pt;
                break;
            }
            case MyParam_my_sfixed32_tag:
            {
                size_t count = my_param.param.my_sfixed32.values_count;
                int32_t *pt = malloc(count * sizeof(int32_t));
                memcpy(pt,
                       my_param.param.my_sfixed32.values,
                       count * sizeof(int32_t));
                result[i] = pt;
                break;
            }
            case MyParam_my_sfixed64_tag:
            {
                size_t count = my_param.param.my_sfixed64.values_count;
                int64_t *pt = malloc(count * sizeof(int64_t));
                memcpy(pt,
                       my_param.param.my_sfixed64.values,
                       count * sizeof(int64_t));
                result[i] = pt;
                break;
            }
            case MyParam_my_bool_tag:
            {
                size_t count = my_param.param.my_bool.values_count;
                bool *pt = malloc(count * sizeof(bool));
                memcpy(pt,
                       my_param.param.my_bool.values,
                       count * sizeof(bool));
                result[i] = pt;
                break;
            }
            case MyParam_my_string_tag:
            {
                size_t len_str = strlen(my_param.param.my_string);
                char *pt = malloc((len_str + 1) * sizeof(char));
                memcpy(pt,
                       my_param.param.my_string,
                       (len_str + 1) * sizeof(char));
                result[i] = pt;
                break;
            }
            case MyParam_my_bytes_tag:
            {
                pb_bytes_array_t *orig = (pb_bytes_array_t*)my_param.param.my_bytes.arg;
                if (orig != NULL)
                {
                    size_t total_size = sizeof(pb_bytes_array_t) + (orig->size - 1) * sizeof(uint8_t);
                    pb_bytes_array_t *copy = malloc(total_size);
                    memcpy(copy, orig, total_size);
                    result[i] = copy;
                }
                else
                {
                    result[i] = NULL;
                }
                break;
            }
            default:
                COGNIT_LOG_DEBUG("Unsupported param type.");
                result[i] = NULL;
                break;
        }
    }
}

void addFC(faas_t* pt_faas, char* fc_code)
{
    strcpy(pt_faas->myfunc.fc_code, fc_code);
    COGNIT_LOG_DEBUG("FC_CODE:\n%s", pt_faas->myfunc.fc_code);
}

int pb_serialize_fc(faas_t* pt_faas, uint8_t* fc_req_buf, int buf_len)
{
    pb_ostream_t stream = pb_ostream_from_buffer(fc_req_buf, (size_t) buf_len);

    if (!pb_encode(&stream, MyFunc_fields, &pt_faas->myfunc))
    {
        COGNIT_LOG_ERROR("Error serializing: %s", PB_GET_ERROR(&stream));
        return 0;
    }
    else
    {
        return (int) stream.bytes_written;
    }
}

int pb_serialize_faas_param(faas_t* pt_faas, int num, uint8_t* req_buf, int len)
{
    pb_ostream_t stream = pb_ostream_from_buffer(req_buf, (size_t) len);

    if (!pb_encode(&stream, MyParam_fields, &pt_faas->params[num]))
    {
        COGNIT_LOG_ERROR("Error serializing: %s", PB_GET_ERROR(&stream));
        return 0;
    }
    else

    {
        return (int) stream.bytes_written;
    }
}

int pb_deserialize_faas_param(uint8_t* res_buf, size_t buf_len, size_t* res_len, void** result)
{
    FaasResponse faas_response = MyParam_init_zero;

    pb_istream_t istream = pb_istream_from_buffer(res_buf, buf_len);

    if (!pb_decode(&istream, FaasResponse_fields, &faas_response))
    {
        COGNIT_LOG_ERROR("Error deserializing: %s", PB_GET_ERROR(&istream));
        return -1;
    }
    else
    {
        *res_len = faas_response.my_faas_response_count;
        parse_response(faas_response, result, *res_len);
        return 0;
    }
}

void pb_parser_init(faas_t* pt_faas)
{
    pt_faas->params_count = 0;
    pt_faas->myfunc = MyFunc_init_zero;
}

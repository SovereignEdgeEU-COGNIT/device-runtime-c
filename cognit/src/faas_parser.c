#include "faas_parser.h"
#include "logger.h"

#define MAX_BYTE_PARAMS 8 // Límite máximo de parámetros de tipo bytes

void printParams(FaasRequest faas_request)
{
    COGNIT_LOG_DEBUG("FaaS Request Params:");
    for (size_t i = 0; i < faas_request.params_count; i++)
    {
        MyParam* param = &faas_request.params[i];

        if (i == faas_request.bytes_pos)
        {
            COGNIT_LOG_DEBUG("  Param[%zu]:", i);
            COGNIT_LOG_DEBUG("    BYTES");
        }
        if (i < faas_request.bytes_pos)
        {
            COGNIT_LOG_DEBUG("  Param[%zu]:", i);
        }
        else
        {
            COGNIT_LOG_DEBUG("  Param[%zu]:", i + 1);
        }
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

bool encode_datos(pb_ostream_t* stream, const pb_field_iter_t* field, void* const* arg)
{
    uint8_t* buffer = (uint8_t*)(*arg);
    size_t length;
    if (buffer == NULL)
    {
        length = 0;
    }
    else
    {
        length = strlen((char*)buffer); // O la longitud real de tus datos
    }

    if (!pb_encode_tag_for_field(stream, field))
    {
        return false;
    }

    return pb_encode_string(stream, buffer, length);
}

void addBYTESParam(faas_t* pt_faas, const uint8_t* bytes, size_t length)
{

    pt_faas->faas_request.my_bytes.arg = bytes;
    pt_faas->faas_request.bytes_pos    = pt_faas->faas_request.params_count;
}

void addSTRINGParam(faas_t* pt_faas, const char* string)
{
    MyParam param     = MyParam_init_zero;
    param.which_param = MyParam_my_string_tag;
    int i             = 0;
    while (string[i] != '\0')
    {
        param.param.my_string[i] = string[i];
        i++;
    }

    pt_faas->faas_request.params[pt_faas->faas_request.params_count++] = param;
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

void parse_response(MyParam response, void** result)
{
    switch (response.which_param)
    {
        case MyParam_my_float_tag:
        {
            *result = response.param.my_float.values;
            COGNIT_LOG_DEBUG("Parsed response as FLOAT: %f", *((float*)*result));
            break;
        }
        case MyParam_my_double_tag:
        {
            *result = response.param.my_double.values;
            COGNIT_LOG_DEBUG("Parsed response as DOUBLE: %lf", *((double*)*result));
            break;
        }
        case MyParam_my_int32_tag:
        {
            *result = response.param.my_int32.values;
            COGNIT_LOG_DEBUG("Parsed response as INT32: %d", *((int32_t*)*result));
            break;
        }
        case MyParam_my_int64_tag:
        {
            *result = response.param.my_int64.values;
            COGNIT_LOG_DEBUG("Parsed response as INT64: %ld", *((int64_t*)*result));
            break;
        }
        case MyParam_my_uint32_tag:
        {
            *result = response.param.my_uint32.values;
            COGNIT_LOG_DEBUG("Parsed response as UINT32: %u", *((uint32_t*)*result));
            break;
        }
        case MyParam_my_uint64_tag:
        {
            *result = response.param.my_uint64.values;
            COGNIT_LOG_DEBUG("Parsed response as UINT64: %lu", *((uint64_t*)*result));
            break;
        }
        case MyParam_my_sint32_tag:
        {
            *result = response.param.my_sint32.values;
            COGNIT_LOG_DEBUG("Parsed response as SINT32: %d", *((int32_t*)*result));
            break;
        }
        case MyParam_my_sint64_tag:
        {
            *result = response.param.my_sint64.values;
            COGNIT_LOG_DEBUG("Parsed response as SINT64: %ld", *((int64_t*)*result));
            break;
        }
        case MyParam_my_fixed32_tag:
        {
            *result = response.param.my_fixed32.values;
            COGNIT_LOG_DEBUG("Parsed response as FIXED32: %u", *((uint32_t*)*result));
            break;
        }
        case MyParam_my_fixed64_tag:
        {
            *result = response.param.my_fixed64.values;
            COGNIT_LOG_DEBUG("Parsed response as FIXED64: %lu", *((uint64_t*)*result));
            break;
        }
        case MyParam_my_sfixed32_tag:
        {
            *result = response.param.my_sfixed32.values;
            COGNIT_LOG_DEBUG("Parsed response as SFIXED32: %d", *((int32_t*)*result));
            break;
        }
        case MyParam_my_sfixed64_tag:
        {
            *result = response.param.my_sfixed64.values;
            COGNIT_LOG_DEBUG("Parsed response as SFIXED64: %ld", *((int64_t*)*result));
            break;
        }
        case MyParam_my_string_tag:
        {
            *result = response.param.my_string;
            COGNIT_LOG_DEBUG("Parsed response as STRING: %s", (char*)*result);
            break;
        }
        default:
            COGNIT_LOG_DEBUG("Unsupported response type.");
            *result = NULL;
            break;
    }
}

void addFC(faas_t* pt_faas, char* fc_name, char* fc_code)
{
    strcpy(pt_faas->myfunc.fc_code, fc_code);
    strcpy(pt_faas->myfunc.fc_hash, "asdf");
    pt_faas->myfunc.fc_id = 0;
    strcpy(pt_faas->myfunc.fc_name, fc_name);

    COGNIT_LOG_DEBUG("FC_NAME: %s", pt_faas->myfunc.fc_name);
    COGNIT_LOG_DEBUG("FC_CODE:\n%s", pt_faas->myfunc.fc_code);
    COGNIT_LOG_DEBUG("FC_ID: %d (still unset)", pt_faas->myfunc.fc_id);
    COGNIT_LOG_DEBUG("FC_HASH: %s (not implemented)\n", pt_faas->myfunc.fc_hash);
}

int faas_serialize_fc(faas_t* pt_faas, uint8_t* fc_req_buf, int buf_len)
{
    pb_ostream_t stream = pb_ostream_from_buffer(fc_req_buf, buf_len);

    if (!pb_encode(&stream, MyFunc_fields, &pt_faas->myfunc))
    {
        COGNIT_LOG_ERROR("Error serializing: %s", PB_GET_ERROR(&stream));
        return 0;
    }
    else
    {
        return stream.bytes_written;
    }
}

int faas_serialize_faas_request(faas_t* pt_faas, uint8_t* req_buf, int len)
{
    printParams(pt_faas->faas_request);
    pb_ostream_t stream = pb_ostream_from_buffer(req_buf, len);

    if (!pb_encode(&stream, FaasRequest_fields, &pt_faas->faas_request))
    {
        COGNIT_LOG_ERROR("Error serializing: %s", PB_GET_ERROR(&stream));
        return 0;
    }
    else

    {
        return stream.bytes_written;
    }
}

void faas_add_fc_id_to_request(faas_t* pt_faas, int fc_id)
{
    pt_faas->faas_request.fc_id = fc_id;
}

int faas_deserialize_fc_upload_response(uint8_t* req_buf, int len)
{

    MyFunc myfunc_response = MyFunc_init_zero;

    pb_istream_t istream = pb_istream_from_buffer(req_buf, len);

    if (!pb_decode(&istream, MyFunc_fields, &myfunc_response))
    {
        COGNIT_LOG_ERROR("Error deserializing: %s", PB_GET_ERROR(&istream));
        return 0;
    }

    COGNIT_LOG_DEBUG("\nResponse received!\n");
    int ret = myfunc_response.fc_id;
    COGNIT_LOG_DEBUG("FC_NAME: %s", myfunc_response.fc_name);
    //COGNIT_LOG_DEBUG("FC_CODE:\n%s", myfunc_response.fc_code);
    COGNIT_LOG_DEBUG("FC_ID: %d", myfunc_response.fc_id);
    COGNIT_LOG_DEBUG("FC_HASH: %s (not implemented)\n", myfunc_response.fc_hash);

    return ret;
}

int faas_deserialize_faas_response(uint8_t* res_buf, int len, void** result)
{
    FaasResponse faas_response = FaasResponse_init_zero;

    pb_istream_t istream = pb_istream_from_buffer(res_buf, len);

    if (!pb_decode(&istream, FaasResponse_fields, &faas_response))
    {
        COGNIT_LOG_ERROR("Error deserializing: %s", PB_GET_ERROR(&istream));
        return -1;
    }
    else
    {
        parse_response(faas_response.res, result);
        return 0;
    }
}

void faas_parser_init(faas_t* pt_faas)
{
    pt_faas->faas_request.my_bytes.arg          = NULL;
    pt_faas->faas_request.my_bytes.funcs.encode = encode_datos;
}

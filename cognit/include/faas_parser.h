#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include "nano.pb.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include <openssl/sha.h>


typedef struct SFaaS
{
    MyFunc myfunc;
    FaasRequest faas_request;
} faas_t;

void faas_parser_init(faas_t* pt_faas);

#define ADD_VAR_FC(FIELD, FIELD_UPPER, TYPE)                \
    void add##FIELD_UPPER##Var(faas_t* pt_faas, TYPE val)   \
    {                                                       \
        TYPE FIELD##_array[1];                              \
        FIELD##_array[0] = val;                             \
        add##FIELD_UPPER##Array(pt_faas, FIELD##_array, 1); \
    }

#define ADD_ARRAY_FC(FIELD, FIELD_UPPER, TYPE)                                      \
    void add##FIELD_UPPER##Array(faas_t* pt_faas, TYPE array[], int length)         \
    {                                                                               \
        MyParam param                       = MyParam_init_zero;                    \
        param.which_param                   = MyParam_my_##FIELD##_tag;             \
        param.param.my_##FIELD.values_count = length;                               \
        for (int i = 0; i < length; i++)                                            \
            param.param.my_##FIELD.values[i] = array[i];                            \
        pt_faas->faas_request.params[pt_faas->faas_request.params_count++] = param; \
    }

void printParams(FaasRequest faas_request);

void addFLOATVar(faas_t* pt_faas_faas, float val);

void addDOUBLEVar(faas_t* pt_faas_faas, double val);

void addINT64Var(faas_t* pt_faas_faas, int64_t val);

void addINT32Var(faas_t* pt_faas_faas, int32_t val);

void addUINT32Var(faas_t* pt_faas_faas, uint32_t val);

void addUINT64Var(faas_t* pt_faas_faas, uint64_t val);

void addSINT32Var(faas_t* pt_faas_faas, int32_t val);

void addSINT64Var(faas_t* pt_faas_faas, int64_t val);

void addFIXED32Var(faas_t* pt_faas_faas, uint32_t val);

void addFIXED64Var(faas_t* pt_faas_faas, uint64_t val);

void addSFIXED32Var(faas_t* pt_faas_faas, int32_t val);

void addSFIXED64Var(faas_t* pt_faas_faas, int64_t val);

//void addBOOLVar(faas_t *pt_faas_faas, protobuf_c_boolean val);

void addFLOATArray(faas_t* pt_faas_faas, float array[], int length);

void addDOUBLEArray(faas_t* pt_faas_faas, double array[], int length);

void addINT64Array(faas_t* pt_faas_faas, int64_t array[], int length);

void addINT32Array(faas_t* pt_faas_faas, int32_t array[], int length);

void addUINT32Array(faas_t* pt_faas_faas, uint32_t array[], int length);

void addUINT64Array(faas_t* pt_faas_faas, uint64_t array[], int length);

void addSINT32Array(faas_t* pt_faas_faas, int32_t array[], int length);

void addSINT64Array(faas_t* pt_faas_faas, int64_t array[], int length);

void addFIXED32Array(faas_t* pt_faas_faas, uint32_t array[], int length);

void addFIXED64Array(faas_t* pt_faas_faas, uint64_t array[], int length);

void addSFIXED32Array(faas_t* pt_faas_faas, int32_t array[], int length);

void addSFIXED64Array(faas_t* pt_faas_faas, int64_t array[], int length);

//void addBOOLArray(faas_t *pt_faas_faas, protobuf_c_boolean array[], int length);

void addBYTESParam(faas_t* pt_faas, const uint8_t* bytes, size_t length);

void addSTRINGParam(faas_t* pt_faas, const char* string);

void addFC(faas_t* pt_faas, char* fc_name, char* fc_code);

int faas_serialize_fc(faas_t* pt_faas, uint8_t* fc_req_buf, int buf_len);

int faas_serialize_faas_request(faas_t* pt_faas, uint8_t* req_buf, int len);

void faas_add_fc_id_to_request(faas_t* pt_faas, int fc_id);

int faas_deserialize_fc_upload_response(uint8_t* req_buf, int len);

int faas_deserialize_faas_response(uint8_t* res_buf, int len, void** result);

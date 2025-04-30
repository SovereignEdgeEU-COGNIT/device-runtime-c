#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include "nano.pb.h"
#include "pb_encode.h"
#include "pb_decode.h"

#define MAX_PARAMS  8

typedef struct SFaaS
{
    MyFunc myfunc;
    int fc_id;
    MyParam params[MAX_PARAMS];
    uint8_t params_count;
} faas_t;

void pb_parser_init(faas_t* pt_faas);

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
            param.param.my_##FIELD.values[i] = array[i];                        \
        if(pt_faas->params_count < MAX_PARAMS - 1)                          \
            pt_faas->params[pt_faas->params_count++] = param;       \
    }

void printParams(faas_t* pt_faas);

void addFLOATVar(faas_t* pt_faas, float val);

void addDOUBLEVar(faas_t* pt_faas, double val);

void addINT64Var(faas_t* pt_faas, int64_t val);

void addINT32Var(faas_t* pt_faas, int32_t val);

void addUINT32Var(faas_t* pt_faas, uint32_t val);

void addUINT64Var(faas_t* pt_faas, uint64_t val);

void addSINT32Var(faas_t* pt_faas, int32_t val);

void addSINT64Var(faas_t* pt_faas, int64_t val);

void addFIXED32Var(faas_t* pt_faas, uint32_t val);

void addFIXED64Var(faas_t* pt_faas, uint64_t val);

void addSFIXED32Var(faas_t* pt_faas, int32_t val);

void addSFIXED64Var(faas_t* pt_faas, int64_t val);

//void addBOOLVar(faas_t *pt_faas, protobuf_c_boolean val);

void addFLOATArray(faas_t* pt_faas, float array[], int length);

void addDOUBLEArray(faas_t* pt_faas, double array[], int length);

void addINT64Array(faas_t* pt_faas, int64_t array[], int length);

void addINT32Array(faas_t* pt_faas, int32_t array[], int length);

void addUINT32Array(faas_t* pt_faas, uint32_t array[], int length);

void addUINT64Array(faas_t* pt_faas, uint64_t array[], int length);

void addSINT32Array(faas_t* pt_faas, int32_t array[], int length);

void addSINT64Array(faas_t* pt_faas, int64_t array[], int length);

void addFIXED32Array(faas_t* pt_faas, uint32_t array[], int length);

void addFIXED64Array(faas_t* pt_faas, uint64_t array[], int length);

void addSFIXED32Array(faas_t* pt_faas, int32_t array[], int length);

void addSFIXED64Array(faas_t* pt_faas, int64_t array[], int length);

//void addBOOLArray(faas_t *pt_faas, protobuf_c_boolean array[], int length);

void addBYTESParam(faas_t* pt_faas, uint8_t* bytes, size_t length);

void addSTRINGParam(faas_t* pt_faas, const char* string);

void addFC(faas_t* pt_faas, char* fc_code);

int pb_serialize_fc(faas_t* pt_faas, uint8_t* fc_req_buf, int buf_len);

int pb_serialize_faas_param(faas_t* pt_faas, uint8_t num, uint8_t* req_buf, int len);

int pb_deserialize_faas_param(uint8_t* res_buf, int len, void** result);

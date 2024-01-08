#ifndef COGNIT_CONFIG_HEADER_
#define COGNIT_CONFIG_HEADER_

/********************** INCLUDES **************************/

#include <stdio.h>
#include <stdint.h>

/***************** DEFINES AND MACROS *********************/

#define COGNIT_ECODE_SUCCESS 0
#define COGNIT_ECODE_ERROR   -1

/**************** TYPEDEFS AND STRUCTS ********************/

typedef struct
{
    const char* prov_engine_endpoint;
    const char* prov_engine_pe_usr;
    const char* prov_engine_pe_pwd;
    uint32_t prov_engine_port;
    uint32_t ui32_serv_runtime_port;
    // Add other fields as needed
} cognit_config_t;

#endif // COGNIT_CONFIG_HEADER_

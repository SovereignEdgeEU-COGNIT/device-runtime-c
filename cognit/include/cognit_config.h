#ifndef COGNIT_CONFIG_HEADER_
#define COGNIT_CONFIG_HEADER_

/********************** INCLUDES **************************/

#include <stdio.h>
#include <stdint.h>

/***************** DEFINES AND MACROS *********************/

#define PE_ENDPOINT_MAX_LEN 256
#define PE_USR_MAX_LEN      256
#define PE_PWD_MAX_LEN      256

#define COGNIT_ECODE_SUCCESS 0
#define COGNIT_ECODE_ERROR   -1

/**************** TYPEDEFS AND STRUCTS ********************/

typedef struct
{
    char prov_engine_endpoint[PE_ENDPOINT_MAX_LEN];
    uint32_t prov_engine_port;
    char prov_engine_pe_usr[PE_USR_MAX_LEN];
    char prov_engine_pe_pwd[PE_PWD_MAX_LEN];
    uint32_t ui32_serv_runtime_port;
    // Add other fields as needed
} cognit_config_t;

#endif // COGNIT_CONFIG_HEADER_

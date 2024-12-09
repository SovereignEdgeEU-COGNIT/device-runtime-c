#ifndef COGNIT_CONFIG_HEADER_
#define COGNIT_CONFIG_HEADER_

/********************** INCLUDES **************************/

#include <stdio.h>
#include <stdint.h>

/***************** DEFINES AND MACROS *********************/

#define COGNIT_ECODE_SUCCESS 0
#define COGNIT_ECODE_ERROR   -1

/**************** TYPEDEFS AND STRUCTS ********************/
/**
 * @brief Structure representing the configuration for the Cognit module.
 */
typedef struct
{
    const char* cognit_frontend_endpoint; /**< The endpoint of the cognit frontend. */
    const char* cognit_frontend_usr;   /**< The username for the provisioning engine. */
    const char* cognit_frontend_pwd;   /**< The password for the provisioning engine. */
    // Add other fields as needed
} cognit_config_t;

#endif // COGNIT_CONFIG_HEADER_
 
# Build the cognit device runtime library

set(SRC_FILES
    "${PROJECT_ROOT}/src/cJSON_Utils.c"
    "${PROJECT_ROOT}/src/cJSON.c"
    "${PROJECT_ROOT}/src/cognit_http.c"
    "${PROJECT_ROOT}/src/prov_engine_cli.c"
    "${PROJECT_ROOT}/src/serverless_runtime_client.c"
    "${PROJECT_ROOT}/src/serverless_runtime_context.c"
    "${PROJECT_ROOT}/src/base64.c"
    "${PROJECT_ROOT}/src/ip_utils.c"
    "${PROJECT_ROOT}/src/faas_parser.c"
    "${PROJECT_ROOT}/src/offload_fc_c.c"
    "${PROJECT_ROOT}/src/sr_parser.c"
    )

set(INCLUDE_PATHS "${PROJECT_ROOT}/include" "${PROJECT_ROOT}/src")

add_library(${PROJECT_NAME} STATIC ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_PATHS})

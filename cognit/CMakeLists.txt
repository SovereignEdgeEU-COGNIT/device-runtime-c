# Build the cognit device runtime library

file(GLOB MBEDTLS_SRC "${PROJECT_ROOT}/mbedtls/*.c")

set(SRC_FILES
    "${PROJECT_ROOT}/src/cJSON_Utils.c"
    "${PROJECT_ROOT}/src/cJSON.c"
    "${PROJECT_ROOT}/src/cognit_http.c"
    "${PROJECT_ROOT}/src/cognit_frontend_cli.c"
    "${PROJECT_ROOT}/src/edge_cluster_frontend_cli.c"
    "${PROJECT_ROOT}/src/device_runtime.c"
    "${PROJECT_ROOT}/src/base64.c"
    "${PROJECT_ROOT}/src/ip_utils.c"
    "${PROJECT_ROOT}/src/device_runtime_state_machine.c"
    "${PROJECT_ROOT}/src/cf_parser.c"
    "${PROJECT_ROOT}/src/faas_parser.c"
    "${PROJECT_ROOT}/src/pb_parser.c"
    "${PROJECT_ROOT}/nanopb/nano.pb.c"
    "${PROJECT_ROOT}/nanopb/pb_common.c"
    "${PROJECT_ROOT}/nanopb/pb_decode.c"
    "${PROJECT_ROOT}/nanopb/pb_encode.c"
    ${MBEDTLS_SRC}
)

set(INCLUDE_PATHS "${PROJECT_ROOT}/include" "${PROJECT_ROOT}/mbedtls" "${PROJECT_ROOT}/mbedtls/library" "${PROJECT_ROOT}/nanopb" "${PROJECT_ROOT}/src")

add_library(${PROJECT_NAME} STATIC ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_PATHS})

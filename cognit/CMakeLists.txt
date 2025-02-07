# Build the cognit device runtime library

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
    "${PROJECT_ROOT}/nanopb/nano.pb.c"
    "${PROJECT_ROOT}/nanopb/pb_common.c"
    "${PROJECT_ROOT}/nanopb/pb_decode.c"
    "${PROJECT_ROOT}/nanopb/pb_encode.c"
    )

set(INCLUDE_PATHS "${PROJECT_ROOT}/include" "${PROJECT_ROOT}/nanopb" "${PROJECT_ROOT}/src")

add_library(${PROJECT_NAME} STATIC ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_PATHS})

# Ruta personalizada para libyaml
set(YAML_INCLUDE_DIR "/usr/include")
set(YAML_LIBRARY "/usr/lib/x86_64-linux-gnu/libyaml.so")

# Verifica si los archivos existen
if (EXISTS ${YAML_INCLUDE_DIR} AND EXISTS ${YAML_LIBRARY})
    message(STATUS "libyaml found manually: ${YAML_INCLUDE_DIR} ${YAML_LIBRARY}")
    target_include_directories(${PROJECT_NAME} PUBLIC ${YAML_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${YAML_LIBRARY})
else()
    message(FATAL_ERROR "libyaml not found. Please install it or set YAML_INCLUDE_DIR and YAML_LIBRARY manually.")
endif()
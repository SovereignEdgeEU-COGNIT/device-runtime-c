set(TEST_SRC_FILES "${PROJECT_ROOT}/src/cognit.c")

set(MINIMAL_OFFLOAD_SYNC_EXAMPLE_TARGET minimal-offload-sync-example)

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

find_path( MBEDTLS_INCLUDE_DIR
           NAMES mbedtls/ssl.h
           HINTS /usr/include
)
find_library( MBEDTLS_LIBRARY    NAMES mbedtls     HINTS /usr/lib/x86_64-linux-gnu )
find_library( MBEDX509_LIBRARY   NAMES mbedx509    HINTS /usr/lib/x86_64-linux-gnu )
find_library( MBEDCRYPTO_LIBRARY NAMES mbedcrypto  HINTS /usr/lib/x86_64-linux-gnu )

if ( NOT MBEDTLS_INCLUDE_DIR OR
     NOT MBEDTLS_LIBRARY OR
     NOT MBEDX509_LIBRARY OR
     NOT MBEDCRYPTO_LIBRARY )
  message(FATAL_ERROR
    "Mbed TLS not found. ¿Is libmbedtls-dev instaled?")
endif()

# Set source files

add_executable(${MINIMAL_OFFLOAD_SYNC_EXAMPLE_TARGET}
               ${EXAMPLES_FOLDER}/minimal-offload-sync-example.c)

target_include_directories(${MINIMAL_OFFLOAD_SYNC_EXAMPLE_TARGET}
                           PRIVATE ${PROJECT_ROOT}/include
                           ${MBEDTLS_INCLUDE_DIR})

target_link_libraries(${MINIMAL_OFFLOAD_SYNC_EXAMPLE_TARGET} ${CURL_LIBRARIES} ${MBEDTLS_LIBRARY} ${MBEDX509_LIBRARY} ${MBEDCRYPTO_LIBRARY}  ${PROJECT_NAME})

add_executable(UC3-offload-sync-example
               ${EXAMPLES_FOLDER}/UC3-offload-sync-example.c)
target_include_directories(UC3-offload-sync-example
  PRIVATE ${PROJECT_ROOT}/include ${MBEDTLS_INCLUDE_DIR})
target_link_libraries(UC3-offload-sync-example
  ${CURL_LIBRARIES} ${MBEDTLS_LIBRARY} ${MBEDX509_LIBRARY} ${MBEDCRYPTO_LIBRARY} ${PROJECT_NAME})

add_custom_target(minimal DEPENDS minimal-offload-sync-example)
add_custom_target(uc3     DEPENDS UC3-offload-sync-example)
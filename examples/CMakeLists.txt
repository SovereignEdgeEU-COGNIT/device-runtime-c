set(TEST_SRC_FILES "${PROJECT_ROOT}/src/cognit.c")

set(MINIMAL_OFFLOAD_SYNC_EXAMPLE_TARGET minimal-offload-sync_example)
set(MINIMAL_OFFLOAD_ASYNC_EXAMPLE_TARGET minimal-offload-async-example)

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})
# Set source files

add_executable(${MINIMAL_OFFLOAD_SYNC_EXAMPLE_TARGET}
               ${EXAMPLES_FOLDER}/minimal-offload-sync-example.c)

add_executable(${MINIMAL_OFFLOAD_ASYNC_EXAMPLE_TARGET}
                minimal-offload-async_example.c)

target_include_directories(${MINIMAL_OFFLOAD_SYNC_EXAMPLE_TARGET}
                           PRIVATE ${PROJECT_ROOT}/include)

target_include_directories(${MINIMAL_OFFLOAD_ASYNC_EXAMPLE_TARGET}
                           PRIVATE ${PROJECT_ROOT}/include)

target_link_libraries(${MINIMAL_OFFLOAD_SYNC_EXAMPLE_TARGET} ${CURL_LIBRARIES} ${PROJECT_NAME})
target_link_libraries(${MINIMAL_OFFLOAD_ASYNC_EXAMPLE_TARGET} ${CURL_LIBRARIES} ${PROJECT_NAME})
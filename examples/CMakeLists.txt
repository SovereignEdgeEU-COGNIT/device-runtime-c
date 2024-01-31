set(TEST_SRC_FILES "${PROJECT_ROOT}/src/cognit.c")

set(MINIMAL_OFFLOAD_EXAMPLE_TARGET minimal-offload-example)

# Set source files

add_executable(${MINIMAL_OFFLOAD_EXAMPLE_TARGET}
               ${EXAMPLES_FOLDER}/minimal-offload-example.c)

target_include_directories(${MINIMAL_OFFLOAD_EXAMPLE_TARGET}
                           PRIVATE ${PROJECT_ROOT}/include)

target_link_libraries(${MINIMAL_OFFLOAD_EXAMPLE_TARGET} ${PROJECT_NAME})

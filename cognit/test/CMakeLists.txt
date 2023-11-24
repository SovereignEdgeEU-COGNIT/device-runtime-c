set(TEST_SRC_FILES "${PROJECT_ROOT}/src/cognit.c")

set(UNIT_TESTS_TARGET unit_tests)

# Set source files
set(GTest_ROOT "/usr/src/googletest/googletest")
add_subdirectory(${GTest_ROOT} "${CMAKE_CURRENT_BINARY_DIR}/gtest")

set(UNIT_TEST_SRC_FILES ${PROJECT_ROOT}/test/unit/test_runner.cpp)
add_executable(${UNIT_TESTS_TARGET} ${UNIT_TEST_SRC_FILES} ${SRC_FILES})

target_include_directories(${UNIT_TESTS_TARGET} PRIVATE ${PROJECT_ROOT}/include)

target_link_libraries(${UNIT_TESTS_TARGET} ${PROJECT_NAME} gtest_main)

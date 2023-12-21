set(TEST_SRC_FILES)

set(UNIT_TESTS_TARGET unit_tests)

# Set source files
set(GTest_ROOT "/usr/src/googletest/googletest")
add_subdirectory(${GTest_ROOT} "${CMAKE_CURRENT_BINARY_DIR}/gtest")

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

set(UNIT_TEST_SRC_FILES 
    ${PROJECT_ROOT}/test/unit/test_runner.cpp
    ${PROJECT_ROOT}/test/unit/utest_http.cpp
    ${PROJECT_ROOT}/test/unit/utest_base64.cpp
    ${PROJECT_ROOT}/test/unit/utest_ip_utils.cpp)
add_executable(${UNIT_TESTS_TARGET} ${UNIT_TEST_SRC_FILES} ${SRC_FILES})

target_include_directories(${UNIT_TESTS_TARGET} PRIVATE ${PROJECT_ROOT}/include ${PROJECT_ROOT}/test/unit)

target_link_libraries(${UNIT_TESTS_TARGET} ${PROJECT_NAME} ${CURL_LIBRARIES} gtest_main)

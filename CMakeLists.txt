project(cognit)

cmake_minimum_required(VERSION 3.5)

# Add a option to compile tests
option(COGNIT_BUILD_TESTS "Build the tests" ON)
option(COGNIT_BUILD_EXAMPLES "Build the examples" ON)

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/cognit)
set(TEST_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/cognit/test)
set(EXAMPLES_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/examples)

# Build the library
add_subdirectory(${PROJECT_ROOT})

# Build tests
if(COGNIT_BUILD_TESTS)
  add_subdirectory(${TEST_FOLDER})
endif()

# Build examples
if(COGNIT_BUILD_EXAMPLES)
  add_subdirectory(${EXAMPLES_FOLDER})
endif()

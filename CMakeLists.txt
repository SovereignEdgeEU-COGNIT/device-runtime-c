project(cognit)

cmake_minimum_required(VERSION 3.5)

if(DEFINED SANITIZER)
  set(SANITIZER ${SANITIZER})
  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -fstack-protector-all  -fstack-check -fsanitize=address -fsanitize=leak  -fno-omit-frame-pointer -g -std=c11 -pedantic -Wall -Wextra -Wpointer-sign -Waddress -Wdiscarded-qualifiers -Wmaybe-uninitialized -Wconversion -Wsign-conversion -Wshadow -Wcast-align -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wundef -Wnested-externs -Wfloat-equal -Wformat=2 -Wformat-security -Wswitch-default -Wswitch-enum -Wstrict-overflow=5 -Winit-self"
  )
  set(CMAKE_C_FLAGS
      "${CMAKE_C_FLAGS} -fstack-check -fstack-protector-all -fsanitize=address  -fsanitize=leak -fno-omit-frame-pointer -g -Wno-comment -std=c11 -pedantic -Wall -Wextra -Wpointer-sign -Waddress -Wdiscarded-qualifiers -Wmaybe-uninitialized -Wconversion -Wsign-conversion -Wshadow -Wcast-align -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wundef -Wnested-externs -Wfloat-equal -Wformat=2 -Wformat-security -Wswitch-default -Wswitch-enum -Wstrict-overflow=5 -Winit-self"
  )
endif()

# Add a option to compile tests
option(COGNIT_BUILD_TESTS "Build the tests" OFF)
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

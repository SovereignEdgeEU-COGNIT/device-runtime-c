# Build the cognit device runtime library

# TODO: Add here the source files
set(SRC_FILES "${PROJECT_ROOT}/src/cognit.c")

set(INCLUDE_PATHS "${PROJECT_ROOT}/include" "${PROJECT_ROOT}/src")

add_library(${PROJECT_NAME} STATIC ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_PATHS})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/testing")

set(SOURCES
    "main.cpp" 
    "catch.hpp"

    "tests/maths.hpp"
)

add_executable(sqee_testing ${SOURCES})
target_link_libraries(sqee_testing sqee)
include_directories("${PROJECT_SOURCE_DIR}/include")

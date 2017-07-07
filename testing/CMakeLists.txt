set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/testing")

set(SOURCES
    "main.cpp"
    "catch.hpp"

    "Common.hpp"
    "Common.cpp"

    "tests/messages.cpp"

    "tests/maths/scalars.cpp"
    "tests/maths/vectors.cpp"
    "tests/maths/matrices.cpp"
    "tests/maths/quaternion.cpp"

    "tests/dop/entries.cpp"
    "tests/dop/joined.cpp"
    "tests/dop/reduce.cpp"
)

add_executable(sqee_testing ${SOURCES})
target_link_libraries(sqee_testing sqee)
include_directories("${PROJECT_SOURCE_DIR}/include")

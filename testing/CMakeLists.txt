project(SQEE_TESTING)

################################################################################

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

################################################################################

add_executable(sqee-testing ${SOURCES})

target_compile_definitions(sqee-testing PRIVATE SQEE_GLOBAL_BUILTINS)

target_link_libraries(sqee-testing PRIVATE sqee)

################################################################################

set_property(TARGET sqee-testing PROPERTY CXX_STANDARD 17)

set_property(TARGET sqee-testing PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/testing")

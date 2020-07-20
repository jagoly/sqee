project(SQEE_TESTING CXX)

################################################################################

file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/*.hpp")
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")

add_executable(sqee-testing ${HEADERS} ${SOURCES})

set_property(TARGET sqee-testing PROPERTY CXX_STANDARD 17)
set_property(TARGET sqee-testing PROPERTY CXX_STANDARD_REQUIRED True)

if (SQEE_GNU OR SQEE_CLANG)
    target_compile_options(sqee-testing PRIVATE -Wall -Wextra)
elseif (SQEE_MSVC)
    target_compile_options(sqee-testing PRIVATE /W3 /wd4201)
endif ()

target_compile_definitions(sqee-testing PRIVATE SQEE_GLOBAL_BUILTINS)

target_link_libraries(sqee-testing PRIVATE sqee)

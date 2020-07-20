project(SQEE_SOKO CXX)

################################################################################

file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/*.hpp")
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")

add_executable(sqee-soko ${HEADERS} ${SOURCES})

set_property(TARGET sqee-soko PROPERTY CXX_STANDARD 17)
set_property(TARGET sqee-soko PROPERTY CXX_STANDARD_REQUIRED True)

if (SQEE_GNU OR SQEE_CLANG)
    target_compile_options(sqee-soko PRIVATE -Wall -Wextra)
elseif (SQEE_MSVC)
    target_compile_options(sqee-soko PRIVATE /W3 /wd4201)
endif ()

target_compile_definitions(sqee-soko PRIVATE SQEE_GLOBAL_BUILTINS)

target_link_libraries(sqee-soko PRIVATE sqee)

################################################################################

set_property(TARGET sqee-soko PROPERTY CXX_STANDARD 17)

set_property(TARGET sqee-soko PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/soko")

add_custom_command(TARGET sqee-soko POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/soko/assets")

add_custom_command(TARGET sqee-soko POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/shaders" "${CMAKE_BINARY_DIR}/soko/shaders")

project(SQEE_SOKO CXX)

# todo: geeze soko is very old and broken
# if I can't bother to fix it, maybe port my space invaders game from splashkit?
# it's good to have a simpler game than rpg included as an example, but this old code is really bad

################################################################################

file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/src/*.hpp")
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable(sqee-soko ${HEADERS} ${SOURCES})

set_property(TARGET sqee-soko PROPERTY CXX_STANDARD 17)
set_property(TARGET sqee-soko PROPERTY CXX_STANDARD_REQUIRED True)

if (SQEE_GNU OR SQEE_CLANG)
    target_compile_options(sqee-soko PRIVATE -Wall -Wextra)
elseif (SQEE_MSVC)
    target_compile_options(sqee-soko PRIVATE /W3 /wd4251)
endif ()

target_compile_definitions(sqee-soko PRIVATE SQEE_GLOBAL_BUILTINS)

target_link_libraries(sqee-soko PRIVATE sqee)

################################################################################

#set_property(TARGET sqee-soko PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/soko")

add_custom_command(TARGET sqee-soko POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink
    "${PROJECT_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/tests/soko/assets")

add_custom_command(TARGET sqee-soko POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink
    "${PROJECT_SOURCE_DIR}/shaders" "${CMAKE_BINARY_DIR}/tests/soko/shaders")

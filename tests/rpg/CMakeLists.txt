project(SQEE_RPG CXX)

# todo: RPG really needs to be moved to it's own repo, it's far too complex to
# function as an example, and it makes the repo much bigger because of assets

################################################################################

file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/src/*.hpp")
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable(sqee-rpg ${HEADERS} ${SOURCES})

set_property(TARGET sqee-rpg PROPERTY CXX_STANDARD 17)
set_property(TARGET sqee-rpg PROPERTY CXX_STANDARD_REQUIRED True)

if (SQEE_GNU OR SQEE_CLANG)
    target_compile_options(sqee-rpg PRIVATE -Wall -Wextra)
elseif (SQEE_MSVC)
    target_compile_options(sqee-rpg PRIVATE /W3 /wd4251)
endif ()

target_compile_definitions(sqee-rpg PRIVATE SQEE_GLOBAL_BUILTINS)

target_link_libraries(sqee-rpg PRIVATE sqee)

################################################################################

#set_property(TARGET sqee-rpg PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests/rpg")

#add_custom_command(TARGET sqee-rpg POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink
#    "${PROJECT_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/tests/rpg/assets")

#add_custom_command(TARGET sqee-rpg POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink
#    "${PROJECT_SOURCE_DIR}/shaders" "${CMAKE_BINARY_DIR}/tests/rpg/shaders")

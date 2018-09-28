project(SQEE_RPG)

################################################################################

file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/*.hpp")
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")

add_executable(sqee-rpg ${HEADERS} ${SOURCES})

target_compile_definitions(sqee-rpg PRIVATE SQEE_GLOBAL_BUILTINS)

target_link_libraries(sqee-rpg PRIVATE sqee)

################################################################################

set_property(TARGET sqee-rpg PROPERTY CXX_STANDARD 17)

set_property(TARGET sqee-rpg PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/rpg")

add_custom_command(TARGET sqee-rpg POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink
    "${PROJECT_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/rpg/assets")

add_custom_command(TARGET sqee-rpg POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink
    "${PROJECT_SOURCE_DIR}/shaders" "${CMAKE_BINARY_DIR}/rpg/shaders")

project(SQEE_SOKO)

################################################################################

set(SOURCES
    "main.cpp"
    "SokoApp.hpp"   "SokoApp.cpp"
    "Game.hpp"      "Game.cpp"
    "Title.hpp"     "Title.cpp"
    "Paused.hpp"    "Paused.cpp"
    "Level.hpp"     "Level.cpp"
)

################################################################################

add_executable(sqee-soko ${SOURCES})

target_compile_definitions(sqee-soko PRIVATE SQEE_GLOBAL_BUILTINS)

target_link_libraries(sqee-soko PRIVATE sqee)

################################################################################

set_property(TARGET sqee-soko PROPERTY CXX_STANDARD 17)

set_property(TARGET sqee-soko PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/soko")

add_custom_command(TARGET sqee-soko POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
                   "${PROJECT_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/soko/assets")

add_custom_command(TARGET sqee-soko POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
                  "${PROJECT_SOURCE_DIR}/shaders" "${CMAKE_BINARY_DIR}/soko/shaders")

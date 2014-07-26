set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/tests/gl2")

set(SOURCES
    "main.cpp"
    "helpers.hpp"
    "app.cpp"           "app.hpp"
    "scenegame.cpp"     "scenegame.hpp"
    "maps.cpp"          "maps.hpp"
    "rendergeneral.cpp" "rendergeneral.hpp"
)

add_executable(gl2 ${SOURCES})
target_link_libraries(gl2 libsqee)
include_directories("${PROJECT_SOURCE_DIR}/include")

add_custom_command(TARGET gl2 COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/build/tests/gl2/res/shaders")
file(GLOB RESOURCES "res/shaders/*.glsl")
foreach(res ${RESOURCES})
    add_custom_command(TARGET gl2 COMMAND ${CMAKE_COMMAND} -E copy ${res} "${CMAKE_BINARY_DIR}/build/tests/gl2/res/shaders")
endforeach()

add_custom_command(TARGET gl2 COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/build/tests/gl2/res/models")
file(GLOB RESOURCES "res/models/*.*")
foreach(res ${RESOURCES})
    add_custom_command(TARGET gl2 COMMAND ${CMAKE_COMMAND} -E copy ${res} "${CMAKE_BINARY_DIR}/build/tests/gl2/res/models")
endforeach()

add_custom_command(TARGET gl2 COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/build/tests/gl2/res/maps")
file(GLOB RESOURCES "res/maps/*.json")
foreach(res ${RESOURCES})
    add_custom_command(TARGET gl2 COMMAND ${CMAKE_COMMAND} -E copy ${res} "${CMAKE_BINARY_DIR}/build/tests/gl2/res/maps")
endforeach()

add_custom_command(TARGET gl2 COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/build/tests/gl2/res/textures/ground")
file(GLOB RESOURCES "res/textures/ground/*.png")
foreach(res ${RESOURCES})
    add_custom_command(TARGET gl2 COMMAND ${CMAKE_COMMAND} -E copy ${res} "${CMAKE_BINARY_DIR}/build/tests/gl2/res/textures/ground")
endforeach()

add_custom_command(TARGET gl2 COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/build/tests/gl2/res/normalmaps/ground")
file(GLOB RESOURCES "res/normalmaps/ground/*.png")
foreach(res ${RESOURCES})
    add_custom_command(TARGET gl2 COMMAND ${CMAKE_COMMAND} -E copy ${res} "${CMAKE_BINARY_DIR}/build/tests/gl2/res/normalmaps/ground")
endforeach()

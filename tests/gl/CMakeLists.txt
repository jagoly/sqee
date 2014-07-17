set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/tests/gl")

set(SOURCES
    "main.cpp"
    "app.cpp"       "app.hpp"
    "scenegame.cpp" "scenegame.hpp"
)

add_executable(gl ${SOURCES})
target_link_libraries(gl libsqee)
include_directories("${PROJECT_SOURCE_DIR}/include")

add_custom_command(TARGET gl COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/build/tests/gl/res")
file(GLOB RESOURCES "res/*.*")
foreach(res ${RESOURCES})
    add_custom_command(TARGET gl COMMAND ${CMAKE_COMMAND} -E copy ${res} "${CMAKE_BINARY_DIR}/build/tests/gl/res")
endforeach()

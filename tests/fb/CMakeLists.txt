set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/tests/fb")

set(SOURCES
    "main.cpp"
    "helpers.hpp"
    "app.cpp"       "app.hpp"
    "scenehud.cpp"  "scenehud.hpp"
    "scenegame.cpp" "scenegame.hpp"
    "menus.cpp"     "menus.hpp"
)

add_executable(fb ${SOURCES})
target_link_libraries(fb libsqee)
include_directories("${PROJECT_SOURCE_DIR}/include")

add_custom_command(TARGET fb COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/build/tests/fb/res")
file(GLOB RESOURCES "res/*.*")
foreach(res ${RESOURCES})
    add_custom_command(TARGET fb COMMAND ${CMAKE_COMMAND} -E copy ${res} "${CMAKE_BINARY_DIR}/build/tests/fb/res")
endforeach()

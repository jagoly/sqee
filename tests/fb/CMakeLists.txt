set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/tests/fb")

set(SOURCES
    "main.cpp"
    "helpers.hpp"
    "app.cpp"       "app.hpp"
    "scenegame.cpp" "scenegame.hpp"
    "menus.cpp"     "menus.hpp"
)

add_executable(fb ${SOURCES})
target_link_libraries(fb libsqee)
include_directories("${PROJECT_SOURCE_DIR}/include")

file(COPY "res" DESTINATION "${CMAKE_BINARY_DIR}/build/tests/fb/")

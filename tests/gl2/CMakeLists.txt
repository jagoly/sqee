set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/tests/gl2")

set(SOURCES
    "main.cpp"
    "helpers.hpp"
    "app.cpp"           "app.hpp"
    "scenegame.cpp"     "scenegame.hpp"
    "maps.cpp"          "maps.hpp"
    "rendergeneral.cpp" "rendergeneral.hpp"
    "player.cpp"        "player.hpp"
)

add_executable(gl2 ${SOURCES})
target_link_libraries(gl2 libsqee)
include_directories("${PROJECT_SOURCE_DIR}/include")

file(COPY "res" DESTINATION "${CMAKE_BINARY_DIR}/build/tests/gl2/")

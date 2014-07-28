set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/tests/gl")

set(SOURCES
    "main.cpp"
    "helpers.hpp"
    "app.cpp"       "app.hpp"
    "scenegame.cpp" "scenegame.hpp"
)

add_executable(gl ${SOURCES})
target_link_libraries(gl libsqee)
include_directories("${PROJECT_SOURCE_DIR}/include")

file(COPY "res" DESTINATION "${CMAKE_BINARY_DIR}/build/tests/gl/")

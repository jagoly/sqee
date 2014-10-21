set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/tests/gl")

set(SOURCES
    "main.cpp"
    "app.cpp"       "app.hpp"
    "scenegame.cpp" "scenegame.hpp"
)

add_executable(gl ${SOURCES})
target_link_libraries(gl sqee)
include_directories("${PROJECT_SOURCE_DIR}/include")

file(COPY "res" DESTINATION "${CMAKE_BINARY_DIR}/build/tests/gl/")

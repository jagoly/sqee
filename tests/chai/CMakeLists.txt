set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/tests/chai")

set(SOURCES
    "main.cpp"
    "app.cpp"       "app.hpp"
    "scenegame.cpp" "scenegame.hpp"
)

add_executable(sqee_chai ${SOURCES})
target_link_libraries(sqee_chai sqee)
include_directories("${PROJECT_SOURCE_DIR}/include")

file(COPY "res" DESTINATION "${CMAKE_BINARY_DIR}/build/tests/chai/")

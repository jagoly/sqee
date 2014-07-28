set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/tests/soko")

set(SOURCES
    "main.cpp"
    "helpers.hpp"
    "app.cpp"             "app.hpp"
    "level.cpp"           "level.hpp"
    "scenegame.cpp"       "scenegame.hpp"
    "gamemenus.cpp"       "gamemenus.hpp"
    "mainmenu.cpp"        "mainmenu.hpp"
)

add_executable(soko ${SOURCES})
target_link_libraries(soko libsqee)
include_directories("${PROJECT_SOURCE_DIR}/include")

file(COPY "res" DESTINATION "${CMAKE_BINARY_DIR}/build/tests/soko/")

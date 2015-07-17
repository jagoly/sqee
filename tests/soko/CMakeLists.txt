set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/tests/soko")

set(SOURCES
    "main.cpp"
    "SokoApp.hpp"   "SokoApp.cpp"
    "Game.hpp"      "Game.cpp"
    "Title.hpp"     "Title.cpp"
    "Paused.hpp"    "Paused.cpp"
    "Level.hpp"     "Level.cpp"
)

add_executable(sqee_soko ${SOURCES})
target_link_libraries(sqee_soko sqee)
include_directories("${PROJECT_SOURCE_DIR}/include")

file(COPY "meshes" DESTINATION "${CMAKE_BINARY_DIR}/build/tests/soko/")
file(COPY "textures" DESTINATION "${CMAKE_BINARY_DIR}/build/tests/soko/")
file(COPY "shaders" DESTINATION "${CMAKE_BINARY_DIR}/build/tests/soko/")

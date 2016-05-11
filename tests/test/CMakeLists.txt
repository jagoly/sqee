set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/tests/test")

set(SOURCES
    "main.cpp"
    "TestApp.hpp"    "TestApp.cpp"
    "TestScene.hpp"  "TestScene.cpp"
    "UIRenderer.hpp" "UIRenderer.cpp"
    "UIShaders.cpp"
)

add_executable(sqee_test ${SOURCES})
target_link_libraries(sqee_test sqee)
include_directories("${PROJECT_SOURCE_DIR}/include")

file(COPY "meshes" DESTINATION "${CMAKE_BINARY_DIR}/build/tests/test/")
file(COPY "shaders" DESTINATION "${CMAKE_BINARY_DIR}/build/tests/test/")

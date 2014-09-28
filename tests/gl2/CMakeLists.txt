set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/tests/gl2")
add_definitions(-DSQEE_DEBUG)

set(SOURCES
    "main.cpp"
    "app.cpp"             "app.hpp"
    "scenegame.cpp"       "scenegame.hpp"
    "rendergeneral.cpp"   "rendergeneral.hpp"
    "player.cpp"          "player.hpp"
    "maps/level.hpp"      "maps/level.cpp"
    "maps/obj/object.hpp" "maps/obj/object.cpp"
    "maps/obj/model.hpp"  "maps/obj/model.cpp"
    "maps/obj/liquid.hpp" "maps/obj/liquid.cpp"
    "maps/obj/data.hpp"   "maps/obj/data.cpp"
    "maps/obj/light.hpp"  "maps/obj/light.cpp"
)

add_executable(gl2 ${SOURCES})
target_link_libraries(gl2 libsqee)
include_directories("${PROJECT_SOURCE_DIR}/include")

file(COPY "res" DESTINATION "${CMAKE_BINARY_DIR}/build/tests/gl2/")

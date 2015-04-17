set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/tests/rpg")

set(SOURCES
    "main.cpp"
    "rpgapp.hpp"                "rpgapp.cpp"
    "player.hpp"                "player.cpp"
    "scenemain.hpp"             "scenemain.cpp"
    "render/graph.hpp"          "render/graph.cpp"
    "render/camera.hpp"         "render/camera.cpp"
    "maps/cell.hpp"             "maps/cell.cpp"
    "maps/world.hpp"            "maps/world.cpp"
    "maps/obj/object.hpp"       "maps/obj/object.cpp"
    "maps/obj/model.hpp"        "maps/obj/model.cpp"
    "maps/obj/liquid.hpp"       "maps/obj/liquid.cpp"
    "maps/obj/reflector.hpp"    "maps/obj/reflector.cpp"
    "maps/obj/light.hpp"        "maps/obj/light.cpp"
)

add_executable(sqee_rpg ${SOURCES})
target_link_libraries(sqee_rpg sqee)
include_directories("${PROJECT_SOURCE_DIR}/include")

file(COPY "assets" DESTINATION "${CMAKE_BINARY_DIR}/build/tests/rpg/")

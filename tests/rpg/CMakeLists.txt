set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/tests/rpg")

set(SOURCES
    "main.cpp"
    "rpgapp.hpp"              "rpgapp.cpp"
    "scenegame.hpp"           "scenegame.cpp"
    "player.hpp"              "player.cpp"
    "render/light.hpp"        "render/light.cpp"
    "render/camera.hpp"       "render/camera.cpp"
    "render/shaders.hpp"      "render/shaders.cpp"
    "entities/model.hpp"      "entities/model.cpp"
    "maps/cell.hpp"           "maps/cell.cpp"
    "maps/world.hpp"          "maps/world.cpp"
    "maps/obj/object.hpp"     "maps/obj/object.cpp"
    "maps/obj/model.hpp"      "maps/obj/model.cpp"
    "maps/obj/liquid.hpp"     "maps/obj/liquid.cpp"
    "maps/obj/reflector.hpp"  "maps/obj/reflector.cpp"
    "maps/obj/light.hpp"      "maps/obj/light.cpp"
)

add_executable(sqee_rpg ${SOURCES})
target_link_libraries(sqee_rpg sqee)
include_directories("${PROJECT_SOURCE_DIR}/include")

file(COPY "res" DESTINATION "${CMAKE_BINARY_DIR}/build/tests/rpg/")

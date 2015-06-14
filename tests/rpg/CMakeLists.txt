set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/tests/rpg")

set(SOURCES
    "main.cpp"
    "rpgapp.hpp"                "rpgapp.cpp"
    "main/scenemain.hpp"        "main/scenemain.cpp"
    "main/handlermain.hpp"      "main/handlermain.cpp"
    "main/scripting.hpp"        "main/scripting.cpp"
    "rndr/graph.hpp"            "rndr/graph_a.cpp"
    "rndr/graph_b.cpp"          "rndr/graph_c.cpp"
    "rndr/graph_d.cpp"          "rndr/graph_e.cpp"
    "wcoe/cell.hpp"             "wcoe/cell.cpp"
    "wcoe/world.hpp"            "wcoe/world.cpp"
    "wcoe/obj/object.hpp"       "wcoe/obj/object.cpp"
    "wcoe/obj/animation.hpp"    "wcoe/obj/animation.cpp"
    "wcoe/obj/modelstatic.hpp"  "wcoe/obj/modelstatic.cpp"
    "wcoe/obj/modelskelly.hpp"  "wcoe/obj/modelskelly.cpp"
    "wcoe/obj/pointlight.hpp"   "wcoe/obj/pointlight.cpp"
    "wcoe/obj/spotlight.hpp"    "wcoe/obj/spotlight.cpp"
    "wcoe/obj/reflector.hpp"    "wcoe/obj/reflector.cpp"
    "wcoe/obj/emitter.hpp"      "wcoe/obj/emitter.cpp"
    "wcoe/obj/liquid.hpp"       "wcoe/obj/liquid.cpp"
    "wcoe/obj/decal.hpp"        "wcoe/obj/decal.cpp"
)

add_executable(sqee_rpg ${SOURCES})
target_link_libraries(sqee_rpg sqee)
include_directories("${PROJECT_SOURCE_DIR}/include")

file(COPY "assets" DESTINATION "${CMAKE_BINARY_DIR}/build/tests/rpg/")
file(COPY "shaders" DESTINATION "${CMAKE_BINARY_DIR}/build/tests/rpg/")

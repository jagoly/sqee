set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/tests/rpg")

set(SOURCES
    "main.cpp"
    "RpgApp.hpp"                "RpgApp.cpp"
    "main/MainScene.hpp"        "main/MainScene.cpp"
    "main/MainHandler.hpp"      "main/MainHandler.cpp"
    "main/Scripting.hpp"        "main/Scripting.cpp"
    "rndr/Graph.hpp"            "rndr/Graph_a.cpp"
    "rndr/Graph_b.cpp"          "rndr/Graph_c.cpp"
    "rndr/Graph_d.cpp"          "rndr/Graph_e.cpp"
    "wcoe/Cell.hpp"             "wcoe/Cell.cpp"
    "wcoe/World.hpp"            "wcoe/World.cpp"
    "wcoe/obj/Object.hpp"       "wcoe/obj/Object.cpp"
    "wcoe/obj/Animation.hpp"    "wcoe/obj/Animation.cpp"
    "wcoe/obj/RigBodyBasic.hpp" "wcoe/obj/RigBodyBasic.cpp"
    "wcoe/obj/RigBodyMulti.hpp" "wcoe/obj/RigBodyMulti.cpp"
    "wcoe/obj/ModelStatic.hpp"  "wcoe/obj/ModelStatic.cpp"
    "wcoe/obj/ModelSkelly.hpp"  "wcoe/obj/ModelSkelly.cpp"
    "wcoe/obj/PointLight.hpp"   "wcoe/obj/PointLight.cpp"
    "wcoe/obj/SpotLight.hpp"    "wcoe/obj/SpotLight.cpp"
    "wcoe/obj/Reflector.hpp"    "wcoe/obj/Reflector.cpp"
    "wcoe/obj/Emitter.hpp"      "wcoe/obj/Emitter.cpp"
    "wcoe/obj/Liquid.hpp"       "wcoe/obj/Liquid.cpp"
    "wcoe/obj/Decal.hpp"        "wcoe/obj/Decal.cpp"
)

add_executable(sqee_rpg ${SOURCES})
target_link_libraries(sqee_rpg sqee)
include_directories("${PROJECT_SOURCE_DIR}/include")

file(COPY "assets" DESTINATION "${CMAKE_BINARY_DIR}/build/tests/rpg/")
file(COPY "shaders" DESTINATION "${CMAKE_BINARY_DIR}/build/tests/rpg/")

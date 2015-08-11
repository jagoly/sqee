set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/tests/rpg")

set(SOURCES
    "main.cpp"
    "RpgApp.hpp"                    "RpgApp.cpp"
    "main/MainScene.hpp"            "main/MainScene.cpp"
    "main/MainHandler.hpp"          "main/MainHandler.cpp"
    "main/Scripting.hpp"            "main/Scripting.cpp"
    "rndr/Graph.hpp"                "rndr/Graph_a.cpp"
    "rndr/Graph_b.cpp"              "rndr/Graph_c.cpp"
    "rndr/Graph_d.cpp"              "rndr/Graph_e.cpp"
    "wcoe/Cell.hpp"                 "wcoe/Cell.cpp"
    "wcoe/World.hpp"                "wcoe/World.cpp"
    "wcoe/Object.hpp"               "wcoe/Object.cpp"
    "wcoe/Animation.hpp"            "wcoe/Animation.cpp"
    "wcoe/objects/MetaObject.hpp"   "wcoe/objects/MetaObject.cpp"
    "wcoe/objects/RigBodyBasic.hpp" "wcoe/objects/RigBodyBasic.cpp"
    "wcoe/objects/RigBodyMulti.hpp" "wcoe/objects/RigBodyMulti.cpp"
    "wcoe/objects/ModelStatic.hpp"  "wcoe/objects/ModelStatic.cpp"
    "wcoe/objects/ModelSkelly.hpp"  "wcoe/objects/ModelSkelly.cpp"
    "wcoe/objects/PointLight.hpp"   "wcoe/objects/PointLight.cpp"
    "wcoe/objects/SpotLight.hpp"    "wcoe/objects/SpotLight.cpp"
    "wcoe/objects/Reflector.hpp"    "wcoe/objects/Reflector.cpp"
    "wcoe/objects/Emitter.hpp"      "wcoe/objects/Emitter.cpp"
    "wcoe/objects/Liquid.hpp"       "wcoe/objects/Liquid.cpp"
    "wcoe/objects/Decal.hpp"        "wcoe/objects/Decal.cpp"
)

add_executable(sqee_rpg ${SOURCES})
target_link_libraries(sqee_rpg sqee)
include_directories("${PROJECT_SOURCE_DIR}/include")

file(COPY "assets" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY "shaders" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

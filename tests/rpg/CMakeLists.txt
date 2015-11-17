set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/tests/rpg")

set(SOURCES
    "main.cpp"
    "RpgApp.hpp"                    "RpgApp.cpp"
    "main/MainScene.hpp"            "main/MainScene.cpp"
    "main/MainHandler.hpp"          "main/MainHandler.cpp"
    "main/Scripting.hpp"            "main/Scripting.cpp"
    "rndr/Renderer.hpp"             "rndr/Renderer.cpp"
    "rndr/Shadows.hpp"              "rndr/Shadows.cpp"
    "rndr/Gbuffers.hpp"             "rndr/Gbuffers.cpp"
    "rndr/Lighting.hpp"             "rndr/Lighting.cpp"
    "rndr/Pretties.hpp"             "rndr/Pretties.cpp"
    "rndr/Reflects.hpp"             "rndr/Reflects.cpp"
    "rndr/Particles.hpp"            "rndr/Particles.cpp"
    "wcoe/Cell.hpp"                 "wcoe/Cell.cpp"
    "wcoe/World.hpp"                "wcoe/World.cpp"
    "wcoe/Object.hpp"               "wcoe/Object.cpp"
    "wcoe/SkyBox.hpp"               "wcoe/SkyBox.cpp"
    "wcoe/Ambient.hpp"              "wcoe/Ambient.cpp"
    "wcoe/SkyLight.hpp"             "wcoe/SkyLight.cpp"
    "wcoe/Animation.hpp"            "wcoe/Animation.cpp"
    "wcoe/objects/MetaObject.hpp"   "wcoe/objects/MetaObject.cpp"
    "wcoe/objects/RigBodyBasic.hpp" "wcoe/objects/RigBodyBasic.cpp"
    "wcoe/objects/RigBodyMulti.hpp" "wcoe/objects/RigBodyMulti.cpp"
    "wcoe/objects/ModelSimple.hpp"  "wcoe/objects/ModelSimple.cpp"
    "wcoe/objects/ModelSkelly.hpp"  "wcoe/objects/ModelSkelly.cpp"
    "wcoe/objects/PointLight.hpp"   "wcoe/objects/PointLight.cpp"
    "wcoe/objects/SpotLight.hpp"    "wcoe/objects/SpotLight.cpp"
    "wcoe/objects/Reflector.hpp"    "wcoe/objects/Reflector.cpp"
    "wcoe/objects/Emitter.hpp"      "wcoe/objects/Emitter.cpp"
    "wcoe/objects/Liquid.hpp"       "wcoe/objects/Liquid.cpp"
    "wcoe/objects/Decal.hpp"        "wcoe/objects/Decal.cpp"

    "components/Transform.hpp"      "components/Transform.cpp"
    "components/Decal.hpp"          "components/Decal.cpp"
    "components/Model.hpp"          "components/Model.cpp"
    "components/SpotLight.hpp"      "components/SpotLight.cpp"
    "components/PointLight.hpp"     "components/PointLight.cpp"
)

add_executable(sqee_rpg ${SOURCES})
target_link_libraries(sqee_rpg sqee)
include_directories("${PROJECT_SOURCE_DIR}/include")

file(COPY "assets" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY "shaders" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

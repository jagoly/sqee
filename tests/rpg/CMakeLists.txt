set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/tests/rpg")

set(SOURCES
    "main.cpp"
    "messages.hpp"
    "helpers.hpp"
    "helpers.cpp"

    "api/editors.hpp"   "api/editors.cpp"
    "api/functions.hpp" "api/functions.cpp"
    "api/other.hpp"     "api/other.cpp"

    "RpgApp.hpp"  "RpgApp.cpp"
    "Options.hpp" "Options.cpp"

    "main/MainScene.hpp"            "main/MainScene.cpp"
    "main/Scripting.hpp"            "main/Scripting.cpp"

    "components/BodyStatic.hpp"     "components/BodyStatic.cpp"
    "components/BodyDynamic.hpp"    "components/BodyDynamic.cpp"
    "components/Decal.hpp"
    "components/LightOrtho.hpp"
    "components/LightPoint.hpp"
    "components/LightSpot.hpp"

    "resources/Manager.hpp"         "resources/Manager.cpp"

    "systems/WorldStuff.hpp" "systems/WorldStuff.cpp"

    "systems/Entity.hpp"     "systems/Entity.cpp"
    "systems/Transform.hpp"  "systems/Transform.cpp"
    "systems/Lighting.hpp"   "systems/Lighting.cpp"
    "systems/Render.hpp"     "systems/Render.cpp"
    "systems/Culling.hpp"    "systems/Culling.cpp"
    "systems/Sound.hpp"      "systems/Sound.cpp"

    "systems/Animation.hpp" "systems/Animation.cpp"
    "systems/Physics.hpp"   "systems/Physics.cpp"

    "world/World.hpp" "world/World.cpp"

    "world/objects/Camera.hpp"
    "world/objects/SkyBox.hpp"
    "world/objects/Ambient.hpp"
    "world/objects/SkyLight.hpp"

    "render/ObjectsData.hpp" "render/ObjectsData.cpp"
    "render/SharedStuff.hpp" "render/SharedStuff.cpp"
    "render/PassesData.hpp"  "render/PassesData.cpp"
    "render/Renderer.hpp"    "render/Renderer.cpp"

    "render/entity/DecalBasic.hpp"  "render/entity/DecalBasic.cpp"
    "render/entity/LightOrtho.hpp"  "render/entity/LightOrtho.cpp"
    "render/entity/LightPoint.hpp"  "render/entity/LightPoint.cpp"
    "render/entity/LightSpot.hpp"   "render/entity/LightSpot.cpp"

    "render/world/Camera.hpp"   "render/world/Camera.cpp"
    "render/world/SkyBox.hpp"   "render/world/SkyBox.cpp"
    "render/world/Ambient.hpp"  "render/world/Ambient.cpp"
    "render/world/SkyLight.hpp" "render/world/SkyLight.cpp"

    "render/passes/DepthDraw.hpp"      "render/passes/DepthDraw.cpp"
    "render/passes/GbufferDraw.hpp"    "render/passes/GbufferDraw.cpp"
    "render/passes/ShadowsDraw.hpp"    "render/passes/ShadowsDraw.cpp"
    "render/passes/LightBaseDraw.hpp"  "render/passes/LightBaseDraw.cpp"
    "render/passes/LightAccumDraw.hpp" "render/passes/LightAccumDraw.cpp"
    "render/passes/VolumetricDraw.hpp" "render/passes/VolumetricDraw.cpp"
    "render/passes/CompositeDraw.hpp"  "render/passes/CompositeDraw.cpp"
    "render/passes/EffectsDraw.hpp"    "render/passes/EffectsDraw.cpp"
)

add_executable(sqee_rpg ${SOURCES})
target_link_libraries(sqee_rpg sqee)
include_directories("${PROJECT_SOURCE_DIR}/include")

file(COPY "assets" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY "shaders" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

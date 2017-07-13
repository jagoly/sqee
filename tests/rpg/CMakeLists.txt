################################################################################

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/tests/rpg")

################################################################################

set(SOURCES

    "main.cpp"

    "RpgApp.hpp"  "RpgApp.cpp"

    "api/editors.hpp"    "api/editors.cpp"
    "api/other.hpp"      "api/other.cpp"
    "api/ScriptAPI.hpp"  "api/ScriptAPI.cpp"

    "main/MainScene.hpp"       "main/MainScene.cpp"
    "main/Options.hpp"         "main/Options.cpp"
    "main/ResourceCaches.hpp"  "main/ResourceCaches.cpp"
    "main/Scripting.hpp"       "main/Scripting.cpp"

    "components/BodyStatic.hpp"   "components/BodyStatic.cpp"
    "components/BodyDynamic.hpp"  "components/BodyDynamic.cpp"

    "systems/Animation.hpp"   "systems/Animation.cpp"
    "systems/Transform.hpp"   "systems/Transform.cpp"
    "systems/Render.hpp"      "systems/Render.cpp"
    "systems/Culling.hpp"     "systems/Culling.cpp"
    "systems/Sound.hpp"       "systems/Sound.cpp"
    "systems/WorldStuff.hpp"  "systems/WorldStuff.cpp"

    "world/Camera.hpp"    "world/Camera.cpp"
    "world/Skybox.hpp"    "world/Skybox.cpp"
    "world/Ambient.hpp"   "world/Ambient.cpp"
    "world/Skylight.hpp"  "world/Skylight.cpp"

    "render/ObjectsData.hpp"  "render/ObjectsData.cpp"
    "render/SharedStuff.hpp"  "render/SharedStuff.cpp"
    "render/PassesData.hpp"   "render/PassesData.cpp"
    "render/Renderer.hpp"     "render/Renderer.cpp"

    "render/entity/DecalBasic.hpp"  "render/entity/DecalBasic.cpp"

    "render/passes/DepthDraw.hpp"       "render/passes/DepthDraw.cpp"
    "render/passes/GbufferDraw.hpp"     "render/passes/GbufferDraw.cpp"
    "render/passes/ShadowsDraw.hpp"     "render/passes/ShadowsDraw.cpp"
    "render/passes/LightBaseDraw.hpp"   "render/passes/LightBaseDraw.cpp"
    "render/passes/LightAccumDraw.hpp"  "render/passes/LightAccumDraw.cpp"
    "render/passes/VolumetricDraw.hpp"  "render/passes/VolumetricDraw.cpp"
    "render/passes/CompositeDraw.hpp"   "render/passes/CompositeDraw.cpp"
    "render/passes/EffectsDraw.hpp"     "render/passes/EffectsDraw.cpp"
)

################################################################################

#file(GLOB_RECURSE SHADERS "shaders/*.glsl")
#file(GLOB_RECURSE SCRIPTS "assets/*.chai")

################################################################################

#add_executable(sqee_rpg ${SOURCES} ${SHADERS} ${SCRIPTS})
add_executable(sqee_rpg ${SOURCES})

target_link_libraries(sqee_rpg sqee)

################################################################################

file(COPY "assets" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY "shaders" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

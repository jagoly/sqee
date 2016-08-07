#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/maths/General.hpp>

#include "../../world/World.hpp"

#include "../../Options.hpp"
#include "SkyLight.hpp"

using namespace sqt::render;
namespace maths = sq::maths;

SkyLightData::SkyLightData()
    : tex(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, sq::Texture::ShadowMap()) {

    ubo.reserve("direction", 3u);
    ubo.reserve("cascadeCount", 1u);
    ubo.reserve("colour", 3u);
    ubo.reserve("cascadeSize", 1u);
    ubo.reserve("matrices", 96u);
    ubo.create_and_allocate();
}

void SkyLightData::refresh(const SceneData& _scene) {

    const world::SkyLightObject& skylight = *_scene.skylight;
    const world::CameraObject& camera = *_scene.camera;

    static const Options& options = Options::get();
    const float rmin = 0.1f, rmax = options.View_Distance;

    cascadeCount = skylight.PROP_cascades;
    cascadeSize = (rmax-rmin) / float(cascadeCount);

    uint resolution = skylight.PROP_resolution;
    resolution *= (options.Shadows_Large + 1u);

    Vec3U newSize {resolution, resolution, cascadeCount};
    if (newSize != tex.get_size()) tex.allocate_storage(newSize, false);

    for (uint i = 0u; i < cascadeCount; i += 1u)
        cascades[i].fbo.attach(gl::DEPTH_ATTACHMENT, tex, i);

    ubo.update("cascadeCount", &cascadeCount);
    ubo.update("cascadeSize", &cascadeSize);


    const Vec3F direction = skylight.PROP_rotation * Vec3F(0.f, 0.f, -1.f);
    const Vec3F tangent = skylight.PROP_rotation * Vec3F(0.f, 1.f, 0.f);

    ubo.update("direction", &direction);
    ubo.update("colour", &skylight.PROP_colour);

    const Vec3F cameraPos = camera.PROP_position;
    const Vec3F cameraDir = camera.PROP_direction;
    float halfRes = float(tex.get_size().x) / 2.f;

    for (uint i = 0u; i < cascadeCount; ++i) {
        const float splitA = rmin + cascadeSize * float(i);
        const float splitB = splitA + cascadeSize;

        Vec3F centre = cameraPos + cameraDir * ((splitA + splitB) / 2.f);
        Mat4F viewMat = maths::look_at(centre - direction, centre, tangent);

        viewMat[3][0] -= std::fmod(viewMat[3][0], splitB / halfRes);
        viewMat[3][1] -= std::fmod(viewMat[3][1], splitB / halfRes);
        viewMat[3][2] -= std::fmod(viewMat[3][2], splitB / halfRes);

        cascades[i].matrix = maths::ortho(Vec3F(-splitB), Vec3F(splitB)) * viewMat;
        cascades[i].ortho = sq::make_Ortho(cascades[i].matrix);

        ubo.update("matrices", i*16u, 16u, &cascades[i].matrix);
    }
}

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/maths/General.hpp>

#include "../../Options.hpp"

#include "LightSpot.hpp"

using namespace sqt::render;
namespace maths = sq::maths;

LightSpotData::LightSpotData(const sq::Entity& _entity)

    : cTransform(*_entity.get_component<TransformComponent>()),
      cLightSpot(*_entity.get_component<LightSpotComponent>()),

      tex(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, sq::Texture::ShadowMap()) {

    ubo.reserve("position", 3u);
    ubo.reserve("intensity", 1u);
    ubo.reserve("direction", 3u);
    ubo.reserve("softness", 1u);
    ubo.reserve("colour", 3u);
    ubo.reserve("angle", 1u);
    ubo.reserve("matrix", 16u);
    ubo.create_and_allocate();
}


void LightSpotData::refresh() {

    if (cLightSpot.PROP_resolution != 0u) {
        uint newSize = cLightSpot.PROP_resolution * (Options::get().Shadows_Large + 1u);
        if (newSize != tex.get_size().x) tex.allocate_storage(Vec2U(newSize), false);
        fbo.attach(gl::DEPTH_ATTACHMENT, tex);
    } else tex.delete_object();

    Vec3F position = Vec3F(cTransform.matrix[3]);
    Vec3F direction = maths::normalize(Mat3F(cTransform.matrix) * Vec3F(0.f, 0.f, -1.f));
    Vec3F tangent = maths::normalize(Mat3F(cTransform.matrix) * Vec3F(0.f, 1.f, 0.f));
    Mat4F viewMat = maths::look_at(position, position + direction, tangent);

    float angle = maths::radians(cLightSpot.PROP_angle);
    float intensity = maths::length(Vec3F(cTransform.matrix[0]));
    matrix = maths::perspective(angle*2.f, 1.f, 0.2f, intensity) * viewMat;
    frus = sq::make_Frustum(matrix, position, direction, 0.2f, intensity);
    Vec3F modelScale = Vec3F(Vec2F(std::tan(angle)), 1.f) * intensity;
    modelMat = maths::scale(maths::inverse(viewMat), modelScale);

    ubo.update("position", &position);
    ubo.update("intensity", &intensity);
    ubo.update("direction", &direction);
    ubo.update("softness", &cLightSpot.PROP_softness);
    ubo.update("colour", &cLightSpot.PROP_colour);
    ubo.update("angle", &angle);
    ubo.update("matrix", &matrix);
}

/*#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/maths/General.hpp>

#include "../../Options.hpp"

#include "../../systems/Transform.hpp"

#include "LightSpot.hpp"

using namespace sqt::render;
namespace maths = sq::maths;

LightSpotData::LightSpotData(const sq::Entity& _entity)
    : cTransform(*_entity.get_component<TransformComponent>()),
      cLightSpot(*_entity.get_component<LightSpotComponent>()),
      entityID(&_entity),
      tex(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, sq::Texture::ShadowMap())
{
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

    const auto tf = sys_Transform().get_transform(entityID);
    const float angle = maths::radians(cLightSpot.PROP_angle);

    // calculate direction and tangent vectors
    Vec3F direction = maths::normalize(Mat3F(tf.matrix) * Vec3F(0.f, 0.f, -1.f));
    Vec3F tangent = maths::normalize(Mat3F(tf.matrix) * Vec3F(0.f, 1.f, 0.f));

    // calculate view and projection matrices
    Mat4F viewMat = maths::look_at(tf.position, tf.position + direction, tangent);
    Mat4F projMat = maths::perspective(angle * 2.f, 1.f, 0.2f, tf.scale);

    matrix = projMat * viewMat;

    // calculate spot light cone bounding volume
    frus = sq::make_Frustum(matrix, tf.position, direction, 0.2f, tf.scale);

    // calculate cone volume size and model matrix
    Vec3F modelScale = Vec3F(Vec2F(std::tan(angle)), 1.f) * tf.scale;
    modelMat = maths::scale(maths::inverse(viewMat), modelScale);

    ubo.update("position", &tf.position);
    ubo.update("intensity", &tf.scale);
    ubo.update("direction", &direction);
    ubo.update("softness", &cLightSpot.PROP_softness);
    ubo.update("colour", &cLightSpot.PROP_colour);
    ubo.update("angle", &angle);
    ubo.update("matrix", &matrix);
}
*/

/*#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/maths/General.hpp>

#include "../../Options.hpp"

#include "../../systems/Transform.hpp"

#include "LightOrtho.hpp"

using namespace sqt::render;
namespace maths = sq::maths;

LightOrthoData::LightOrthoData(const sq::Entity& _entity)
    : cTransform(*_entity.get_component<TransformComponent>()),
      cLightOrtho(*_entity.get_component<LightOrthoComponent>()),
      entityID(&_entity),
      tex(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, sq::Texture::ShadowMap())
{
    ubo.reserve("direction", 4u);
    ubo.reserve("density", 1u);
    ubo.reserve("colour", 3u);
    ubo.reserve("matrix", 16u);
    ubo.create_and_allocate();
}


void LightOrthoData::refresh() {

    if (cLightOrtho.PROP_resolution != 0u) {
        uint newSize = cLightOrtho.PROP_resolution * (Options::get().Shadows_Large + 1u);
        if (newSize != tex.get_size().x) tex.allocate_storage(Vec2U(newSize), false);
        fbo.attach(gl::DEPTH_ATTACHMENT, tex);
    } else tex.delete_object();

    const auto tf = sys_Transform().get_transform(entityID);

    // calculate direction and tangent vectors
    Vec3F direction = maths::normalize(Mat3F(tf.matrix) * Vec3F(0.f, 0.f, -1.f));
    Vec3F tangent = maths::normalize(Mat3F(tf.matrix) * Vec3F(0.f, 1.f, 0.f));

    Vec3F scaledMinimum = cLightOrtho.PROP_minimum * tf.scale;
    Vec3F scaledMaximum = cLightOrtho.PROP_maximum * tf.scale;

    // calculate view and projection matrices
    Mat4F viewMat = maths::look_at(tf.position, tf.position + direction, tangent);
    Mat4F projMat = maths::ortho(scaledMinimum, scaledMaximum);

    matrix = projMat * viewMat;

    // calculate model matrix and bounding volume
    modelMat = maths::inverse(matrix);
    ortho = sq::make_Ortho(matrix);

    ubo.update("direction", &direction);
    ubo.update("density", &cLightOrtho.PROP_density);
    ubo.update("colour", &cLightOrtho.PROP_colour);
    ubo.update("matrix", &matrix);
}
*/

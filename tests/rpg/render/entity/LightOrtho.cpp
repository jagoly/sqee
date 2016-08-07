#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/maths/General.hpp>

#include "../../Options.hpp"

#include "LightOrtho.hpp"

using namespace sqt::render;
namespace maths = sq::maths;

LightOrthoData::LightOrthoData(const sq::Entity& _entity)

    : cTransform(*_entity.get_component<TransformComponent>()),
      cLightOrtho(*_entity.get_component<LightOrthoComponent>()),

      tex(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, sq::Texture::ShadowMap()) {

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

    Vec3F position = Vec3F(cTransform.matrix[3]);
    float scale = maths::length(Vec3F(cTransform.matrix[0]));

    Vec3F direction = maths::normalize(Mat3F(cTransform.matrix) * Vec3F(0.f, 0.f, -1.f));
    Vec3F tangent = maths::normalize(Mat3F(cTransform.matrix) * Vec3F(0.f, 1.f, 0.f));

    Vec3F scaledMinimum = cLightOrtho.PROP_minimum * scale;
    Vec3F scaledMaximum = cLightOrtho.PROP_maximum * scale;

    matrix = maths::look_at(position, position + direction, tangent);
    matrix = maths::ortho(scaledMinimum, scaledMaximum) * matrix;
    modelMat = maths::inverse(matrix);

    ortho = sq::make_Ortho(matrix);

    ubo.update("direction", &direction);
    ubo.update("density", &cLightOrtho.PROP_density);
    ubo.update("colour", &cLightOrtho.PROP_colour);
    ubo.update("matrix", &matrix);
}

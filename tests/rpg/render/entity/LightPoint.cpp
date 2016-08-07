#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/maths/General.hpp>

#include "../../Options.hpp"

#include "LightPoint.hpp"

using namespace sqt::render;
namespace maths = sq::maths;

LightPointData::LightPointData(const sq::Entity& _entity)

    : cTransform(*_entity.get_component<TransformComponent>()),
      cLightPoint(*_entity.get_component<LightPointComponent>()),

      tex(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, sq::Texture::ShadowMap()) {

    ubo.reserve("position", 3u);
    ubo.reserve("intensity", 1u);
    ubo.reserve("colour", 4u);
    ubo.reserve("matrices", 96u);
    ubo.create_and_allocate();
}


void LightPointData::refresh() {

    if (cLightPoint.PROP_resolution != 0u) {
        uint newSize = cLightPoint.PROP_resolution * (Options::get().Shadows_Large + 1u);
        if (newSize != tex.get_size().x) tex.allocate_storage(newSize, false);
        for (uint i = 0u; i < 6u; ++i) faces[i].fbo.attach(gl::DEPTH_ATTACHMENT, tex, i);
    } else tex.delete_object();

    Vec3F position = Vec3F(cTransform.matrix[3]);
    float intensity = maths::length(Vec3F(cTransform.matrix[0]));
    modelMat = maths::scale(maths::translate(Mat4F(), position), Vec3F(intensity));
    sphere.origin = position; sphere.radius = intensity;

    ubo.update("position", &position);
    ubo.update("intensity", &intensity);
    ubo.update("colour", &cLightPoint.PROP_colour);

    if (cLightPoint.PROP_resolution != 0u) {

        const Mat4F projMat = maths::perspective(maths::radians(0.25f), 1.f, 0.1f, intensity);

        faces[0].matrix = projMat * maths::look_at(position, position + Vec3F(+1, 0, 0), Vec3F(0, -1, 0));
        faces[1].matrix = projMat * maths::look_at(position, position + Vec3F(-1, 0, 0), Vec3F(0, -1, 0));
        faces[2].matrix = projMat * maths::look_at(position, position + Vec3F(0, +1, 0), Vec3F(0, 0, +1));
        faces[3].matrix = projMat * maths::look_at(position, position + Vec3F(0, -1, 0), Vec3F(0, 0, -1));
        faces[4].matrix = projMat * maths::look_at(position, position + Vec3F(0, 0, +1), Vec3F(0, -1, 0));
        faces[5].matrix = projMat * maths::look_at(position, position + Vec3F(0, 0, -1), Vec3F(0, -1, 0));

        faces[0].frus = sq::make_Frustum(faces[0].matrix, position, Vec3F(+1, 0, 0), 0.1f, intensity);
        faces[1].frus = sq::make_Frustum(faces[1].matrix, position, Vec3F(-1, 0, 0), 0.1f, intensity);
        faces[2].frus = sq::make_Frustum(faces[2].matrix, position, Vec3F(0, +1, 0), 0.1f, intensity);
        faces[3].frus = sq::make_Frustum(faces[3].matrix, position, Vec3F(0, -1, 0), 0.1f, intensity);
        faces[4].frus = sq::make_Frustum(faces[4].matrix, position, Vec3F(0, 0, +1), 0.1f, intensity);
        faces[5].frus = sq::make_Frustum(faces[5].matrix, position, Vec3F(0, 0, -1), 0.1f, intensity);

        array<Mat4F, 6> matrices { faces[0].matrix, faces[1].matrix, faces[2].matrix,
                                   faces[3].matrix, faces[4].matrix, faces[5].matrix };

        ubo.update("matrices", matrices.data());
    }
}

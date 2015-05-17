#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/maths/culling.hpp>
#include <sqee/maths/general.hpp>

#include "../cell.hpp"
#include "../world.hpp"
#include "spotlight.hpp"

using namespace sqt::wcoe;

SpotLight::SpotLight(const string& _name, const Cell* _cell)
    : Object(ObjType::SpotLight, _name, _cell) {
    ubo.reset(new sq::Uniformbuffer());
    ubo->reserve("position", 3);
    ubo->reserve("angle", 1);
    ubo->reserve("direction", 3);
    ubo->reserve("intensity", 1);
    ubo->reserve("colour", 3);
    ubo->reserve("softness", 1);
    ubo->reserve("matrix", 16);
    ubo->reserve("modelMat", 16);
    ubo->create();
}

void SpotLight::load_from_spec(const ObjSpec& _spec) {
    DAT_shadow = _spec.flags.count("shadow");
    DAT_specular = _spec.flags.count("specular");

    DAT_position  = glm::make_vec3(_spec.fMap.at("position").data());
    DAT_direction = glm::make_vec3(_spec.fMap.at("direction").data());
    DAT_colour    = glm::make_vec3(_spec.fMap.at("colour").data());
    DAT_angle     = _spec.fMap.at("angle")[0];
    DAT_intensity = _spec.fMap.at("intensity")[0];
    DAT_softness  = _spec.fMap.at("softness")[0];
    if (DAT_shadow) DAT_texsize = _spec.iMap.at("texsize")[0];
}

void SpotLight::update_from_data() {
    vec3 position = DAT_position + cell->position;
    float angle = glm::radians(DAT_angle);

    vec3 tangent = sq::make_tangent(DAT_direction);
    mat4 viewMat = glm::lookAt(position, position+DAT_direction, tangent);
    matrix = glm::perspective(2.f*angle, 1.f, 0.2f, DAT_intensity) * viewMat;
    frus = sq::make_Frustum(matrix, position, DAT_direction, {0.2, DAT_intensity});

    float tanAngle = glm::tan(angle*2.f);
    vec3 scale = -vec3(tanAngle, tanAngle, 1.f) * DAT_intensity;
    modelMat = glm::inverse(viewMat) * glm::scale(mat4(), scale);

    if (DAT_shadow == true) {
        tex.reset(new sq::Texture2D());
        tex->create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, 1);
        tex->set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);
        fbo.reset(new sq::Framebuffer()); fbo->attach(gl::DEPTH_ATTACHMENT, *tex);
    } else { tex.reset(); fbo.reset(); }

    ubo->bind(1);
    ubo->update("position", &position);
    ubo->update("angle", &angle);
    ubo->update("direction", &DAT_direction);
    ubo->update("intensity", &DAT_intensity);
    ubo->update("colour", &DAT_colour);
    ubo->update("softness", &DAT_softness);
    ubo->update("matrix", &matrix);
    ubo->update("modelMat", &modelMat);
}

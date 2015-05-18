#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sqee/app/logging.hpp>
#include <glm/gtx/string_cast.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/render/mesh.hpp>
#include <sqee/render/skin.hpp>
#include <sqee/maths/culling.hpp>
#include <sqee/maths/general.hpp>

#include "../cell.hpp"
#include "reflector.hpp"

using namespace sqt::wcoe;

Reflector::Reflector(const string& _name, const Cell* _cell)
    : Object(ObjType::Reflector, _name, _cell) {
    ubo.reset(new sq::Uniformbuffer());
    ubo->reserve("matrix", 16);
    ubo->reserve("normal", 4);
    ubo->reserve("trans", 3);
    ubo->reserve("factor", 1);
    ubo->create();

    texDepth.reset(new sq::Texture2D());
    texDiff.reset(new sq::Texture2D());
    texSurf.reset(new sq::Texture2D());
    texHdr.reset(new sq::Texture2D());
    texDepth->create(gl::DEPTH_STENCIL, gl::DEPTH24_STENCIL8, 1);
    texDiff->create(gl::RGB, gl::RGB8, 3);
    texSurf->create(gl::RGB, gl::RGB8, 3);
    texHdr->create(gl::RGB, gl::RGB16F, 3);
    texDepth->set_preset(sq::Texture2D::L_C());
    texDiff->set_preset(sq::Texture2D::L_C());
    texSurf->set_preset(sq::Texture2D::L_C());
    texHdr->set_preset(sq::Texture2D::L_C());

    fboDefr.reset(new sq::Framebuffer());
    fboHdr.reset(new sq::Framebuffer());
    fboDefr->attach(gl::DEPTH_STENCIL_ATTACHMENT, *texDepth);
    fboDefr->attach(gl::COLOR_ATTACHMENT0, *texDiff);
    fboDefr->attach(gl::COLOR_ATTACHMENT1, *texSurf);
    fboHdr->attach(gl::DEPTH_STENCIL_ATTACHMENT, *texDepth);
    fboHdr->attach(gl::COLOR_ATTACHMENT0, *texHdr);
}

void Reflector::load_from_spec(const ObjSpec& _spec) {
    DAT_shadow = _spec.flags.count("shadow");
    DAT_normal = glm::make_vec3(_spec.fMap.at("normal").data());
    DAT_factor = _spec.fMap.at("factor")[0];
    DAT_mPath = _spec.sMap.at("mesh")[0];
    DAT_sPath = _spec.sMap.at("skin")[0];
    if (_spec.fMap.count("pos")) DAT_pos = glm::make_vec3(_spec.fMap.at("pos").data());
    if (_spec.fMap.count("rot")) DAT_rot = glm::make_vec3(_spec.fMap.at("rot").data());
    if (_spec.fMap.count("sca")) DAT_sca = glm::make_vec3(_spec.fMap.at("sca").data());
}

void Reflector::update_from_data() {
    if (!(mesh = sq::res::mesh().get(DAT_mPath)))
        mesh = sq::res::mesh().add(DAT_mPath),
        mesh->create(DAT_mPath);

    if (!(skin = sq::res::skin().get(DAT_sPath)))
        skin = sq::res::skin().add(DAT_sPath),
        skin->create(DAT_sPath);

    trans = DAT_pos + cell->position;
    matrix = glm::translate(mat4(), trans);
    matrix = glm::rotate(matrix, glm::radians(DAT_rot.x), {1,0,0});
    matrix = glm::rotate(matrix, glm::radians(DAT_rot.y), {0,1,0});
    matrix = glm::rotate(matrix, glm::radians(DAT_rot.z), {0,0,1});
    matrix = glm::scale(matrix, DAT_sca);
    negScale = glm::determinant(matrix) < 0.f;
    bbox = sq::make_BoundBox(matrix, mesh->origin, mesh->size, mesh->radius);
    normal = glm::normalize(sq::make_normMat(matrix) * vec3(0,0,1));

    ubo->bind(2);
    ubo->update("matrix", &matrix);
    ubo->update("normal", &normal);
    ubo->update("trans", &trans);
    ubo->update("factor", &DAT_factor);
}

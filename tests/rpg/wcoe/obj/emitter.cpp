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
#include "emitter.hpp"

using namespace sqt::wcoe;

Emitter::Emitter(const string& _name, Cell* _cell)
    : Object(ObjType::Emitter, _name, _cell) {
    ubo.reset(new sq::Uniformbuffer());
    ubo->reserve("matrix", 16);
    ubo->create();

    texDepth.reset(new sq::Texture2D());
    texDiff.reset(new sq::Texture2D());
    texNorm.reset(new sq::Texture2D());
    texHdr.reset(new sq::Texture2D());
    texDepth->create(gl::DEPTH_STENCIL, gl::DEPTH24_STENCIL8, 1);
    texDiff->create(gl::RGBA, gl::RGBA12, 4);
    texNorm->create(gl::RG, gl::RG16, 2);
    texHdr->create(gl::RGB, gl::RGB16F, 3);
    texDepth->set_preset(sq::Texture2D::L_C());
    texDiff->set_preset(sq::Texture2D::L_C());
    texNorm->set_preset(sq::Texture2D::L_C());
    texHdr->set_preset(sq::Texture2D::L_C());
    fboDefr.reset(new sq::Framebuffer());
    fboDefr->attach(gl::DEPTH_STENCIL_ATTACHMENT, *texDepth);
    fboDefr->attach(gl::COLOR_ATTACHMENT0, *texDiff);
    fboDefr->attach(gl::COLOR_ATTACHMENT1, *texNorm);
    fboHdr->attach(gl::DEPTH_STENCIL_ATTACHMENT, *texDepth);
    fboHdr->attach(gl::COLOR_ATTACHMENT0, *texHdr);
}

void Emitter::load_from_spec(const ObjSpec& _spec) {
    DAT_position = glm::make_vec3(_spec.fMap.at("position").data());
}

void Emitter::refresh() {
    vec3 position = DAT_position + cell->DAT_position;
    matrix = glm::translate(mat4(), position);
    sphere = {position, DAT_radius};

    ubo->bind(1);
    ubo->update("matrix", &matrix);
}

void Emitter::tick() {}

void Emitter::calc(double _accum) {}

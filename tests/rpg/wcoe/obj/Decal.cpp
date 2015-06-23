#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/maths/Culling.hpp>
#include <sqee/maths/General.hpp>

#include "../Cell.hpp"
#include "../World.hpp"
#include "Decal.hpp"

using namespace sqt::wcoe;

Decal::Decal(const string& _name, Cell* _cell)
    : Object(ObjType::Decal, _name, _cell) {
    ubo.reset(new sq::UniformBuffer());
    ubo->reserve("matrix", 16);
    ubo->reserve("invMat", 16);
    ubo->reserve("d_n_s", 3);
    ubo->reserve("alpha", 1);
    ubo->create();
}

void Decal::load_from_spec(const ObjSpec& _spec) {
    SPEC_ASSERT_FLOAT("position", 3);
    SPEC_ASSERT_FLOAT("rotation", 4);
    SPEC_ASSERT_FLOAT("scale", 3);

    PROP_position = glm::make_vec3(_spec.fMap.at("position").data());
    PROP_rotation = glm::make_quat(_spec.fMap.at("rotation").data());
    PROP_scale    = glm::make_vec3(_spec.fMap.at("scale").data());
    SPEC_IF_STRING("diff") PROP_diffPath = _spec.sMap.at("diff")[0];
    SPEC_IF_STRING("norm") PROP_normPath = _spec.sMap.at("norm")[0];
    SPEC_IF_STRING("spec") PROP_specPath = _spec.sMap.at("spec")[0];
}

void Decal::refresh() {
    if (PROP_diffPath.empty() == false) {
        const string name = "decals/" + PROP_diffPath + "_d";
        if ((texDiff = sq::res::tex2D().get(name)) == nullptr) {
            texDiff = sq::res::tex2D().add(name);
            texDiff->create(gl::RGBA, gl::RGBA8, 4);
            texDiff->set_preset(sq::Texture2D::M_C());
            texDiff->buffer_file(name);
            texDiff->gen_mipmap();
        }
    } else texDiff = nullptr;

    if (PROP_normPath.empty() == false) {
        const string name = "decals/" + PROP_normPath + "_n";
        if ((texNorm = sq::res::tex2D().get(name)) == nullptr) {
            texNorm = sq::res::tex2D().add(name);
            texNorm->create(gl::RGBA, gl::RGBA8, 4);
            texNorm->set_preset(sq::Texture2D::M_C());
            texNorm->buffer_file(name);
            texNorm->gen_mipmap();
        }
    } else texNorm = nullptr;

    if (PROP_specPath.empty() == false) {
        const string name = "decals/" + PROP_specPath + "_s";
        if ((texSpec = sq::res::tex2D().get(name)) == nullptr) {
            texSpec = sq::res::tex2D().add(name);
            texSpec->create(gl::RGBA, gl::RGBA8, 4);
            texSpec->set_preset(sq::Texture2D::M_C());
            texSpec->buffer_file(name);
            texSpec->gen_mipmap();
        }
    } else texSpec = nullptr;

    ivec3 dns = {bool(texDiff), bool(texNorm), bool(texSpec)};
    ubo->bind(1); ubo->update("d_n_s", &dns);

    animate();
}

void Decal::tick() {
    bool doFinish = false;
    if (ANIM_position.active()) if (ANIM_position.tick()) doFinish = true;
    if (ANIM_rotation.active()) if (ANIM_rotation.tick()) doFinish = true;
    if (ANIM_scale.active())    if (ANIM_scale.tick())    doFinish = true;
    if (ANIM_alpha.active())    if (ANIM_alpha.tick())    doFinish = true;
    if (doFinish == true) animate();
}

void Decal::calc(double _accum) {
    bool doAnim = false;
    if (ANIM_position.active()) { ANIM_position.calc(_accum); doAnim = true; }
    if (ANIM_rotation.active()) { ANIM_rotation.calc(_accum); doAnim = true; }
    if (ANIM_scale.active())    { ANIM_scale.calc(_accum);    doAnim = true; }
    if (ANIM_alpha.active())    { ANIM_alpha.calc(_accum);    doAnim = true; }
    if (doAnim == true) animate();
}

void Decal::animate() {
    fvec3 position = PROP_position + cell->DAT_position;
    matrix = glm::translate(fmat4(), position);
    matrix *= glm::mat4_cast(PROP_rotation);
    matrix = glm::scale(matrix, PROP_scale);
    invMat = glm::inverse(matrix);
    bbox = sq::make_BoundBox(matrix, {0,0,0}, 0.87f, {1,1,1});

    ubo->bind(1);
    ubo->update("matrix", &matrix);
    ubo->update("invMat", &invMat);
    ubo->update("alpha", &PROP_alpha);
}

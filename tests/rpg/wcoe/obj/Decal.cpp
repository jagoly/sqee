#include <glm/gtc/matrix_transform.hpp>

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
   #ifdef SQEE_DEBUG
    assert_fvec3(_spec, name, "position");
    assert_fquat(_spec, name, "rotation");
    assert_fvec3(_spec, name, "scale");
   #endif

    PROP_position = _spec.fvec3Map.at("position");
    PROP_rotation = _spec.fquatMap.at("rotation");
    PROP_scale    = _spec.fvec3Map.at("scale");

    if (_spec.stringMap.count("diff")) PROP_diff = _spec.stringMap.at("diff");
    if (_spec.stringMap.count("norm")) PROP_norm = _spec.stringMap.at("norm");
    if (_spec.stringMap.count("spec")) PROP_spec = _spec.stringMap.at("spec");
}

void Decal::refresh() {
    if (PROP_diff.empty() == false) {
        const string name = "decals/" + PROP_diff + "_d";
        if ((texDiff = sq::res::tex2D().get(name)) == nullptr) {
            texDiff = sq::res::tex2D().add(name);
            texDiff->create(gl::RGBA, gl::RGBA8, 4);
            texDiff->set_preset(sq::Texture2D::M_C());
            texDiff->buffer_file(name);
            texDiff->gen_mipmap();
        }
    } else texDiff = nullptr;

    if (PROP_norm.empty() == false) {
        const string name = "decals/" + PROP_norm + "_n";
        if ((texNorm = sq::res::tex2D().get(name)) == nullptr) {
            texNorm = sq::res::tex2D().add(name);
            texNorm->create(gl::RGBA, gl::RGBA8, 4);
            texNorm->set_preset(sq::Texture2D::M_C());
            texNorm->buffer_file(name);
            texNorm->gen_mipmap();
        }
    } else texNorm = nullptr;

    if (PROP_spec.empty() == false) {
        const string name = "decals/" + PROP_spec + "_s";
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

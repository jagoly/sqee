#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/app/Resources.hpp>
#include <sqee/gl/Textures.hpp>

#include "../Cell.hpp"
#include "../World.hpp"
#include "Decal.hpp"

using namespace sqt::wcoe;
namespace maths = sq::maths;

Decal::Decal(const string& _name, Cell* _cell)
    : Object(typeid(Decal), _name, _cell) {

    ubo.reserve("matrix", 16u);
    ubo.reserve("invMat", 16u);
    ubo.reserve("d_n_s", 3u);
    ubo.reserve("alpha", 1u);
    ubo.allocate_storage();
}

void Decal::load_from_spec(const ObjSpec& _spec) {
    _spec.set_if("position", PROP_position);
    _spec.set_if("rotation", PROP_rotation);
    _spec.set_if("scale", PROP_scale);
    _spec.set_if("alpha", PROP_alpha);
    _spec.set_if("diff", PROP_diff);
    _spec.set_if("norm", PROP_norm);
    _spec.set_if("spec", PROP_spec);
}

void Decal::refresh() {
    if (invalid == false) return;

    if (PROP_diff.empty() == false) {
        const string name = "decals/" + PROP_diff + "_d";
        if ((texDiff = sq::static_Texture2D().get(name)) == nullptr) {
            texDiff = sq::static_Texture2D().add(name, gl::RGBA, gl::RGBA8, sq::Texture::MipmapClamp());
            texDiff->buffer_auto(name, true);
        }
    } else texDiff = nullptr;

    if (PROP_norm.empty() == false) {
        const string name = "decals/" + PROP_norm + "_n";
        if ((texNorm = sq::static_Texture2D().get(name)) == nullptr) {
            texNorm = sq::static_Texture2D().add(name, gl::RGBA, gl::RGBA8, sq::Texture::MipmapClamp());
            texNorm->buffer_auto(name, true);
        }
    } else texNorm = nullptr;

    if (PROP_spec.empty() == false) {
        const string name = "decals/" + PROP_spec + "_s";
        if ((texSpec = sq::static_Texture2D().get(name)) == nullptr) {
            texSpec = sq::static_Texture2D().add(name, gl::RGBA, gl::RGBA8, sq::Texture::MipmapClamp());
            texSpec->buffer_auto(name, true);
        }
    } else texSpec = nullptr;

    Vec3I dns{bool(texDiff), bool(texNorm), bool(texSpec)};
    ubo.update("d_n_s", &dns);

    animate(); invalid = false;
}

void Decal::update() {
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
    matrix = maths::translate(Mat4F(), PROP_position + cell->PROP_position);
    matrix *= Mat4F(PROP_rotation); matrix = maths::scale(matrix, PROP_scale);
    bbox = sq::make_BoundBox(matrix, {0.f, 0.f, 0.f}, 0.87f, {1.f, 1.f, 1.f});
    invMat = maths::inverse(matrix);

    ubo.update("matrix", &matrix);
    ubo.update("invMat", &invMat);
    ubo.update("alpha", &PROP_alpha);
}

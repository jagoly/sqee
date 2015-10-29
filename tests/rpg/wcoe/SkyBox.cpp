#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/app/Resources.hpp>
#include <sqee/render/Camera.hpp>

#include "World.hpp"
#include "SkyBox.hpp"

using namespace sqt::wcoe;

SkyBox::SkyBox(const World& _world) :
    tex(gl::RGB, gl::RGB8, sq::Texture::LinearClamp()),
    world(_world) {

    ubo.reserve("saturation", 1u);
    ubo.reserve("brightness", 1u);
    ubo.reserve("contrast", 1u);
    ubo.reserve("alpha", 1u);
    ubo.allocate_storage();
}

void SkyBox::refresh() {
    if (PROP_enabled == false) return;

    string texPath = "skybox/" + PROP_texture;
    tex.allocate_storage(1024u, false);
    tex.buffer_full(texPath);

    animate();
}

void SkyBox::update() {
    if (PROP_enabled == false) return; bool doFinish = false;
    if (ANIM_saturation.active() && ANIM_saturation.tick()) doFinish = true;
    if (ANIM_brightness.active() && ANIM_brightness.tick()) doFinish = true;
    if (ANIM_contrast.active()   && ANIM_contrast.tick())   doFinish = true;
    if (ANIM_alpha.active()      && ANIM_alpha.tick())      doFinish = true;
    if (doFinish == true) animate();
}

void SkyBox::calc(double _accum) {
    if (PROP_enabled == false) return; bool doAnim = false;
    if (ANIM_saturation.active()) { ANIM_saturation.calc(_accum); doAnim = true; }
    if (ANIM_brightness.active()) { ANIM_brightness.calc(_accum); doAnim = true; }
    if (ANIM_contrast.active())   { ANIM_contrast.calc(_accum);   doAnim = true; }
    if (ANIM_alpha.active())      { ANIM_alpha.calc(_accum);      doAnim = true; }
    if (doAnim == true) animate();
}

void SkyBox::animate() {
    ubo.update("saturation", &PROP_saturation);
    ubo.update("brightness", &PROP_brightness);
    ubo.update("contrast", &PROP_contrast);
    ubo.update("alpha", &PROP_alpha);
}

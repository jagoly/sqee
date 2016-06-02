#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/app/Resources.hpp>

#include "SkyBox.hpp"

using namespace sqt;

SkyBox::SkyBox() :
    tex(gl::RGB, gl::RGB8, sq::Texture::LinearClamp()) {

    ubo.reserve("saturation", 1u);
    ubo.reserve("brightness", 1u);
    ubo.reserve("contrast", 1u);
    ubo.reserve("alpha", 1u);
    ubo.create_and_allocate();
}

void SkyBox::configure(const World& _world) {
    if (currentTex != PROP_texture) {
        tex.allocate_storage(1024u, false);
        tex.buffer_full("skybox/" + PROP_texture);
        currentTex = PROP_texture;
    }
}

void SkyBox::update(const World& _world) {
    ubo.update("saturation", &PROP_saturation);
    ubo.update("brightness", &PROP_brightness);
    ubo.update("contrast", &PROP_contrast);
    ubo.update("alpha", &PROP_alpha);
}

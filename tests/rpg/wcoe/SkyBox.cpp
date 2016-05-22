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

void SkyBox::configure() {
    if (PROP_enabled == false) return;

    string texPath = "skybox/" + PROP_texture;
    tex.allocate_storage(1024u, false);
    tex.buffer_full(texPath);
}

void SkyBox::update() {
    ubo.update("saturation", &PROP_saturation);
    ubo.update("brightness", &PROP_brightness);
    ubo.update("contrast", &PROP_contrast);
    ubo.update("alpha", &PROP_alpha);
}

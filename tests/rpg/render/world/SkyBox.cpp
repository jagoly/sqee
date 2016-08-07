#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/maths/General.hpp>

#include "SkyBox.hpp"

using namespace sqt::render;
namespace maths = sq::maths;

SkyBoxData::SkyBoxData()
    : tex(gl::RGB, gl::RGB8, sq::Texture::LinearClamp()) {
}

void SkyBoxData::refresh(const world::SkyBoxObject& _obj) {
    if (path != _obj.PROP_texture) {
        tex.allocate_storage(1024u, false);
        tex.buffer_full(_obj.PROP_texture);
        path = _obj.PROP_texture;
    }


    params.r = _obj.PROP_saturation; params.g = _obj.PROP_brightness;
    params.b = _obj.PROP_contrast; params.a = _obj.PROP_opacity;
}

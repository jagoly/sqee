#include <sqee/redist/gl_ext_4_2.hpp>

#include "SkyBox.hpp"

using namespace sqt::render;
namespace maths = sq::maths;

SkyBoxData::SkyBoxData()
    : tex(sq::Texture::Format::RGB8_UN)
{
    tex.set_filter_mode(true);
}

void SkyBoxData::refresh(const world::SkyBoxObject& _obj)
{
    if (path != _obj.PROP_texture)
    {
        tex.allocate_storage(1024u);
        tex.load_file("textures/" + _obj.PROP_texture + "/0_right",   0u);
        tex.load_file("textures/" + _obj.PROP_texture + "/1_left",    1u);
        tex.load_file("textures/" + _obj.PROP_texture + "/2_forward", 2u);
        tex.load_file("textures/" + _obj.PROP_texture + "/3_back",    3u);
        tex.load_file("textures/" + _obj.PROP_texture + "/4_up",      4u);
        tex.load_file("textures/" + _obj.PROP_texture + "/5_down",    5u);
        path = _obj.PROP_texture;
    }

    params.r = _obj.PROP_saturation; params.g = _obj.PROP_brightness;
    params.b = _obj.PROP_contrast; params.a = _obj.PROP_opacity;
}

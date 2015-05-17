#pragma once

#include "object.hpp"

namespace sqt { namespace wcoe {

class Reflector : public Object {
public:
    Reflector(const string& _name, const Cell* _cell);
    void load_from_spec(const ObjSpec& _spec);
    void update_from_data();

    vec3 DAT_pos = {0,0,0};
    vec3 DAT_rot = {0,0,0};
    vec3 DAT_sca = {1,1,1};
    vec3 DAT_normal = {0,0,1};
    bool DAT_shadow = false;
    string DAT_mPath;
    string DAT_sPath;

    unique_ptr<sq::Uniformbuffer> ubo;
    unique_ptr<sq::Texture2D> texDepth;
    unique_ptr<sq::Texture2D> texDiff;
    unique_ptr<sq::Texture2D> texSurf;
    unique_ptr<sq::Texture2D> texHdr;
    unique_ptr<sq::Framebuffer> fboDefr;
    unique_ptr<sq::Framebuffer> fboHdr;
    mat4 matrix;
    vec3 normal;
    vec3 trans;

    sq::Mesh* mesh = nullptr;
    sq::Skin* skin = nullptr;
    bool negScale;
    sq::BoundBox bbox;
};

template<> struct ObjTraits<Reflector> {
    static constexpr ObjType type() { return ObjType::Reflector; }
};

}}

#pragma once

#include "object.hpp"

namespace sqt { namespace wcoe {

class Reflector : public Object {
public:
    Reflector(const string& _name, Cell* _cell);
    void load_from_spec(const ObjSpec& _spec);
    void refresh(); void tick();
    void calc(double _accum);

    fvec3 DAT_position = {0.f, 0.f, 0.f};
    fquat DAT_rotation = {1.f, 0.f, 0.f, 0.f};
    fvec3 DAT_scale    = {1.f, 1.f, 1.f};
    fvec3 DAT_normal   = {0.f, 0.f, 1.f};
    float DAT_factor   = 1.f;
    bool DAT_shadow    = false;
    string DAT_mPath   = "";
    string DAT_sPath   = "";

    unique_ptr<sq::Uniformbuffer> ubo;
    unique_ptr<sq::Texture2D> texDepth;
    unique_ptr<sq::Texture2D> texDiff;
    unique_ptr<sq::Texture2D> texSurf;
    unique_ptr<sq::Texture2D> texHdr;
    unique_ptr<sq::Framebuffer> fboDefr;
    unique_ptr<sq::Framebuffer> fboHdr;
    fvec3 trans, normal;
    fmat4 matrix;

    sq::Mesh* mesh = nullptr;
    sq::Skin* skin = nullptr;
    bool negScale;
    sq::BoundBox bbox;
};

template<> struct ObjTraits<Reflector> {
    static constexpr ObjType type() { return ObjType::Reflector; }
};

}}

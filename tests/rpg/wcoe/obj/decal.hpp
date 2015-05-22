#pragma once

#include "object.hpp"

namespace sqt { namespace wcoe {

class Decal : public Object {
public:
    Decal(const string& _name, Cell* _cell);
    void load_from_spec(const ObjSpec& _spec);
    void refresh(); void tick();
    void calc(double _accum);

    vec3 DAT_position = {0, 0, 0};
    vec3 DAT_normal = {0, 0, -1};
    vec3 DAT_tangent = {0, 1, 0};
    vec3 DAT_scale = {1, 1, 1};

    string DAT_dPath;
    string DAT_nPath;
    string DAT_sPath;

    unique_ptr<sq::Uniformbuffer> ubo;
    sq::Texture2D* texDiff = nullptr;
    sq::Texture2D* texNorm = nullptr;
    sq::Texture2D* texSpec = nullptr;
    mat4 matrix, invMat;
    sq::BoundBox bbox;
};

template<> struct ObjTraits<Decal> {
    static constexpr ObjType type() { return ObjType::Decal; }
};

}}

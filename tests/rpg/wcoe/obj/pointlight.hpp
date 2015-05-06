#pragma once

#include "object.hpp"

namespace sqt { namespace wcoe {

class PointLight : public Object {
public:
    PointLight(const string& _name, const Cell* _cell);
    void load_from_spec(const ObjSpec& _spec);
    void update_from_data();

    bool DAT_shadow = false;
    bool DAT_specular = false;
    vec3 DAT_position = {0, 0, 0};
    vec3 DAT_colour = {1, 1, 1};
    float DAT_intensity = 10.f;
    uint DAT_texsize = 0u;

    array<mat4, 6> matArr;
    sq::Sphere sphere;
    array<sq::Frustum, 6> frusArr;
    unique_ptr<sq::Uniformbuffer> ubo;
    unique_ptr<sq::TextureCube> tex;
    array<unique_ptr<sq::Framebuffer>, 6> fboArr;
};

template<> struct ObjTraits<PointLight> {
    static constexpr ObjType type() { return ObjType::PointLight; }
};

}}

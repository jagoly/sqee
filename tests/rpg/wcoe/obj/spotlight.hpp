#pragma once

#include "object.hpp"

namespace sqt { namespace wcoe {

class SpotLight : public Object {
public:
    SpotLight(const string& _name, Cell* _cell);
    void load_from_spec(const ObjSpec& _spec);
    void refresh(); void tick();
    void calc(double _accum);

    bool DAT_shadow = false;
    bool DAT_specular = false;
    vec3 DAT_position = {0, 0, 0};
    vec3 DAT_direction = {0, 0, -1};
    vec3 DAT_colour = {1, 1, 1};
    float DAT_angle = 45.f;
    float DAT_intensity = 10.f;
    float DAT_softness = 0.f;
    uint DAT_texsize = 0u;

    mat4 matrix;
    unique_ptr<sq::Uniformbuffer> ubo;
    unique_ptr<sq::Texture2D> tex;
    unique_ptr<sq::Framebuffer> fbo;
    sq::Frustum frus; mat4 modelMat;
};

template<> struct ObjTraits<SpotLight> {
    static constexpr ObjType type() { return ObjType::SpotLight; }
};

}}

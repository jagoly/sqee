#pragma once

#include "object.hpp"

namespace sqt { namespace wcoe {

class ModelSkelly : public Object {
public:
    ModelSkelly(const string& _name, Cell* _cell);
    void load_from_spec(const ObjSpec& _spec);
    void refresh(); void tick();
    void calc(double _accum);

    bool   PROP_render   = false;
    bool   PROP_shadow   = false;
    fvec3  PROP_position = {0.f, 0.f, 0.f};
    fquat  PROP_rotation = {1.f, 0.f, 0.f, 0.f};
    fvec3  PROP_scale    = {1.f, 1.f, 1.f};
    string PROP_mPath    = "";
    string PROP_sPath    = "";

    // animators...
    void animate();

    unique_ptr<sq::Uniformbuffer> ubo;
    unique_ptr<sq::Skeleton> skel;
    sq::Mesh* mesh = nullptr;
    sq::Skin* skin = nullptr;
    sq::BoundBox bbox;
    bool negScale;
    fmat4 matrix;

    void debug_A();
    void debug_B();
    void debug_C();
    void debug_D();
};

template<> struct ObjTraits<ModelSkelly> {
    static constexpr ObjType type() { return ObjType::ModelSkelly; }
};

}}

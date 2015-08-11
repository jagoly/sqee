#pragma once

#include "../Object.hpp"

namespace sqt { namespace wcoe {

class Emitter : public Object {
public:
    Emitter(const string& _name, Cell* _cell);
    void load_from_spec(const ObjSpec& _spec);
    void refresh(); void tick();
    void calc(double _accum);

    fvec3 PROP_position = {0, 0, 0};

    // ANIMS
    void animate();

    struct Particle {
        Particle(fvec3 _origin, fvec3 _target, fvec3 _colour, float _scale, uint _life);
        const fvec3 origin, target, colour; const float scale; const uint life;
        fvec4 crntA, crntB, nextA, nextB; uint progress;
    }; struct PartData { float x, y, z, s, r, g, b, a; };

    forward_list<Particle> particleList;
    forward_list<PartData> partDataList;

    void emit_puff(uint _count,
                   fvec3 _normal, fvec3 _colour,
                   float _scaleMin, float _scaleMax,
                   float _radiusMin, float _radiusMax,
                   uint _lifeMin, uint _lifeMax);

    unique_ptr<sq::UniformBuffer> ubo;
    fvec3 position;
};

template<> struct ObjTraits<Emitter> {
    static constexpr ObjType type() { return ObjType::Emitter; }
};

}}

#pragma once
#include <forward_list>

#include <sqee/gl/UniformBuffer.hpp>

#include "../Object.hpp"

namespace sqt { namespace wcoe {

class Emitter final : public Object {
public:
    Emitter(const string& _name, Cell* _cell);

    void load_from_spec(const ObjSpec& _spec);

    void refresh(), update();
    void calc(double _accum);
    void animate();

    fvec3 PROP_position = {0, 0, 0};

    void FUNC_emit_puff(uint _count,
                        fvec3 _normal, fvec3 _colour,
                        float _scaleMin, float _scaleMax,
                        float _radiusMin, float _radiusMax,
                        uint _lifeMin, uint _lifeMax);

    struct Particle {
        Particle(fvec3 _origin, fvec3 _target, fvec3 _colour, float _scale, uint _life);
        const fvec3 origin, target, colour; const float scale; const uint life;
        fvec4 crntA, crntB, nextA, nextB; uint progress;
    }; struct PartData { float x, y, z, s, r, g, b, a; };

    std::forward_list<Particle> particleList;
    std::forward_list<PartData> partDataList;

    sq::UniformBuffer ubo;
    fvec3 position;
};

}}

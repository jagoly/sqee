#pragma once

#include <forward_list>

#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/maths/Vectors.hpp>

#include "../Object.hpp"

namespace sqt { namespace wcoe {

class Emitter final : public Object {
public:
    Emitter(const string& _name, Cell* _cell);

    void load_from_spec(const ObjSpec& _spec);

    void refresh(), update();
    void calc(double _accum);
    void animate();

    Vec3F PROP_position = {0.f, 0.f, 0.f};

    void FUNC_emit_puff(uint _count,
                        Vec3F _normal, Vec3F _colour,
                        float _scaleMin, float _scaleMax,
                        float _radiusMin, float _radiusMax,
                        uint _lifeMin, uint _lifeMax);

    struct Particle {
        Particle(Vec3F _origin, Vec3F _target, Vec3F _colour, float _scale, uint _life);
        const Vec3F origin, target, colour; const float scale; const uint life;
        Vec4F crntA, crntB, nextA, nextB; uint progress;
    }; struct PartData { float x, y, z, s, r, g, b, a; };

    std::forward_list<Particle> particleList;
    std::forward_list<PartData> partDataList;

    sq::UniformBuffer ubo;
    Vec3F position;
};

}}

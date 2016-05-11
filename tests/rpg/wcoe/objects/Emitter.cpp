/*#include <random>

#include "../Cell.hpp"
#include "../World.hpp"
#include "Emitter.hpp"

using namespace sqt::wcoe;
namespace maths = sq::maths;

Emitter::Emitter(const string& _name, Cell* _cell)
    : Object(typeid(Emitter), _name, _cell) {

    ubo.reserve("matrix", 16u);
    ubo.create_and_allocate();
}

void Emitter::load_from_spec(const ObjSpec& _spec) {
    _spec.set_if("position", PROP_position);
}


Emitter::Particle::Particle(Vec3F _origin, Vec3F _target, Vec3F _colour, float _scale, uint _life)
    : origin(_origin), target(_target), colour(_colour), scale(_scale), life(_life) {
    crntA = nextA = Vec4F(origin.x, origin.y, origin.z, scale * 0.5f);
    crntB = nextB = Vec4F(colour.r, colour.g, colour.b, 1.f);
}


void Emitter::FUNC_emit_puff(uint _count,
                             Vec3F _normal, Vec3F _colour,
                             float _scaleMin, float _scaleMax,
                             float _radiusMin, float _radiusMax,
                             uint _lifeMin, uint _lifeMax) {
    std::random_device rd; std::mt19937 gen(rd());
    std::uniform_int_distribution<uint> distLife(_lifeMin, _lifeMax);
    std::uniform_real_distribution<float> distScale(_scaleMin, _scaleMax);
    std::uniform_real_distribution<float> distRadius(_radiusMin, _radiusMax);
    std::uniform_real_distribution<float> distZO(0.f, 1.f);

    for (uint i = 0u; i < _count; i++) {
        float z = distZO(gen) * 2.f - 1.f;
        float azimuth = distZO(gen) * 2.f * 3.1415927f;
        float x = std::cos(azimuth) * std::sqrt(1.f-z*z);
        float y = std::sin(azimuth) * std::sqrt(1.f-z*z);
        Vec3F normal = maths::normalize(Vec3F(x, y, z) + _normal);

        float life = distLife(gen);
        float scale = distScale(gen);
        float radius = distRadius(gen);

        particleList.emplace_front(PROP_position + cell->PROP_position,
                                   PROP_position + cell->PROP_position
                                   + normal * radius, _colour, scale, life);
    }
}


void Emitter::refresh() {
    if (invalid == false) return;

    position = PROP_position + cell->PROP_position;

    animate(); invalid = false;
}


void Emitter::update() {
    for (auto& p : particleList) { p.crntA = p.nextA; p.crntB = p.nextB;
        float percent = float(++p.progress) / p.life; p.nextB.a = 1.f - percent;
        Vec3F pos = maths::mix(p.origin, p.target, (std::sqrt(percent) + percent) / 2.f);
        p.nextA.x = pos.x; p.nextA.y = pos.y; p.nextA.z = pos.z;
        p.nextA.w = maths::mix(0.5f * p.scale, p.scale, percent);
    } particleList.remove_if([](Particle& p) { return p.progress == p.life; });
}


void Emitter::calc(double _accum) {
    partDataList.clear();
    for (auto& p : particleList) {
        Vec4F datA = maths::mix(p.crntA, p.nextA, float(_accum) * 24.f);
        Vec4F datB = maths::mix(p.crntB, p.nextB, float(_accum) * 24.f);
        PartData pd = {datA.x, datA.y, datA.z, datA.w, datB.r, datB.g, datB.b, datB.a};
        partDataList.emplace_front(pd);
    }
}


void Emitter::animate() {}
*/

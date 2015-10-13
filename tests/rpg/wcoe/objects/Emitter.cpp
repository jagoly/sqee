#include <random>

#include <glm/gtc/matrix_transform.hpp>

#include <sqee/maths/Culling.hpp>
#include <sqee/maths/General.hpp>

#include "../Cell.hpp"
#include "../World.hpp"
#include "Emitter.hpp"

using namespace sqt::wcoe;

Emitter::Emitter(const string& _name, Cell* _cell) : Object(_name, _cell) {
    ubo.reserve("matrix", 16u);
    ubo.allocate_storage();
}

void Emitter::load_from_spec(const ObjSpec& _spec) {
    _spec.set_if("position", PROP_position);
}


Emitter::Particle::Particle(fvec3 _origin, fvec3 _target, fvec3 _colour, float _scale, uint _life)
    : origin(_origin), target(_target), colour(_colour), scale(_scale), life(_life) {
    crntA = nextA = fvec4(origin.x, origin.y, origin.z, scale * 0.5f);
    crntB = nextB = fvec4(colour.r, colour.g, colour.b, 1.f);
}


void Emitter::FUNC_emit_puff(uint _count,
                             fvec3 _normal, fvec3 _colour,
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
        float x = glm::cos(azimuth) * glm::sqrt(1.f-z*z);
        float y = glm::sin(azimuth) * glm::sqrt(1.f-z*z);
        fvec3 normal = glm::normalize(fvec3(x, y, z) + _normal);

        float life = distLife(gen);
        float scale = distScale(gen);
        float radius = distRadius(gen);

        particleList.emplace_front(PROP_position + cell->PROP_position,
                                   PROP_position + cell->PROP_position
                                   + normal * radius, _colour, scale, life);
    }
}


void Emitter::refresh() {
    if (revalidate() == true) {
        position = PROP_position + cell->PROP_position;
    }

    animate();
}


void Emitter::update() {
    for (auto& p : particleList) { p.crntA = p.nextA; p.crntB = p.nextB;
        float percent = float(++p.progress) / p.life; p.nextB.a = 1.f - percent;
        fvec3 pos = glm::mix(p.origin, p.target, (glm::sqrt(percent) + percent) / 2.f);
        p.nextA.x = pos.x; p.nextA.y = pos.y; p.nextA.z = pos.z;
        p.nextA.w = glm::mix(0.5f * p.scale, p.scale, percent);
    } particleList.remove_if([](Particle& p) { return p.progress == p.life; });
}


void Emitter::calc(double _accum) {
    partDataList.clear();
    for (auto& p : particleList) {
        fvec4 datA = glm::mix(p.crntA, p.nextA, _accum * 24.f);
        fvec4 datB = glm::mix(p.crntB, p.nextB, _accum * 24.f);
        PartData pd = {datA.x, datA.y, datA.z, datA.w,
                       datB.r, datB.g, datB.b, datB.a};
        partDataList.emplace_front(pd);
    }
}


void Emitter::animate() {}

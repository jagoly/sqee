#pragma once

#include <sqee/gl/uniformbuffers.hpp>

#include "object.hpp"

namespace sqt {

class Liquid : public Object {
public:
    Liquid(const ObjSpec& _spec);

    float wScale;
    float wSmooth;
    vec2 flow;
    vec3 colour;
    float thickness;

    GLuint vao;
    GLuint vboP, vboTc;
    sq::Texture* normArray;

    sq::Uniformbuffer ubo;

    void update_ubo();
    void bind_ubo();
    void draw();

    void tick();
    void calc(double _accum);

private:
    vec3 normInfo;
    vec2 flowOffs;
    float normOffsA, normOffsB;
    vec2 flowOffsA, flowOffsB;

    sq::Mesh* mesh;
};


template<> struct ObjTraits<Liquid> {
    static constexpr ObjType type() { return ObjType::Liquid; }
};

}

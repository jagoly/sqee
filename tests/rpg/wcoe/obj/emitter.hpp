#pragma once

#include "object.hpp"

namespace sqt { namespace wcoe {

class Emitter : public Object {
public:
    Emitter(const string& _name, Cell* _cell);
    void load_from_spec(const ObjSpec& _spec);
    void refresh(); void tick();
    void calc(double _accum);

    fvec3 DAT_position = {0, 0, 0};
    float DAT_radius = 1.f;

    void emit_Sphere(uint _count, fvec4 _colour,
                     float _lifeMin, float _lifeMax,
                     float _lumaMin, float _lumaMax,
                     float _scaleMin, float _scaleMax,
                     float _radiusMin, float _radiusMax);

    unique_ptr<sq::Uniformbuffer> ubo;
    unique_ptr<sq::Texture2D> texDepth;
    unique_ptr<sq::Texture2D> texDiff;
    unique_ptr<sq::Texture2D> texNorm;
    unique_ptr<sq::Texture2D> texHdr;
    unique_ptr<sq::Framebuffer> fboDefr;
    unique_ptr<sq::Framebuffer> fboHdr;
    fmat4 matrix; sq::Sphere sphere;

    struct Particle {
        Particle(fvec3 _target, fvec3 _colour, float _life, float _scale);
            //: target(_target), colour(_colour), life(_life), scale(_scale) {}
        const fvec3 target, colour; const float life, scale;
        float progress; fvec3 crntPos; float crntAlpha;
    };

    struct EmitSphere {
        EmitSphere(uint _count, fvec4 _colour);
            //: count(_count), colour(_colour) {}
        const uint count; const fvec4 colour;
        forward_list<Particle> particleList;
        GLuint vboColS, vboPosA, vao;
    };
};

template<> struct ObjTraits<Emitter> {
    static constexpr ObjType type() { return ObjType::Emitter; }
};

}}

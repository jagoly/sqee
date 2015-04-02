#pragma once
#include <sqee/forward.hpp>

namespace sq {

class Box {
public:
    Box() = default;
    Box(const float* _bounds);
    float xA, xB, yA, yB, zA, zB;
};

class ModBox {
public:
    void init(const Box& _base);

    void reset();
    void recalc();
    void translate(vec3 _pos);
    void rotate(vec3 _rot);
    void scale(vec3 _sca);

    float xO, yO, zO;
    array<float, 3> p000, p001, p010, p011, p100, p101, p110, p111;

    bool intersect_pnt(vec3 _pos);
    bool intersect_box(const Box& _box);

    Box base;
    Box box;
};

}

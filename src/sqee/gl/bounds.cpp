#include <gl/bounds.hpp>

using namespace sq;

Box::Box(const float* _bounds)
    : xA(_bounds[0]), xB(_bounds[1]), yA(_bounds[2]),
      yB(_bounds[3]), zA(_bounds[4]), zB(_bounds[5]) {
}


void ModBox::init(const Box& _base) {
    base = _base;
    reset();
}

void ModBox::reset() {
    xO = 0; yO = 0; zO = 0;
    p000 = {base.xA, base.yA, base.zA};
    p001 = {base.xA, base.yA, base.zB};
    p010 = {base.xA, base.yB, base.zA};
    p011 = {base.xA, base.yB, base.zB};
    p100 = {base.xB, base.yA, base.zA};
    p101 = {base.xB, base.yA, base.zB};
    p110 = {base.xB, base.yB, base.zA};
    p111 = {base.xB, base.yB, base.zB};
    box = base;
}

void ModBox::recalc() {
    box.xA =                  p000[0] ; box.xB =                  p000[0];
    box.xA = std::min(box.xA, p001[0]); box.xB = std::max(box.xB, p001[0]);
    box.xA = std::min(box.xA, p010[0]); box.xB = std::max(box.xB, p010[0]);
    box.xA = std::min(box.xA, p011[0]); box.xB = std::max(box.xB, p011[0]);
    box.xA = std::min(box.xA, p100[0]); box.xB = std::max(box.xB, p100[0]);
    box.xA = std::min(box.xA, p101[0]); box.xB = std::max(box.xB, p101[0]);
    box.xA = std::min(box.xA, p110[0]); box.xB = std::max(box.xB, p110[0]);
    box.xA = std::min(box.xA, p111[0]); box.xB = std::max(box.xB, p111[0]);

    box.yA =                  p000[1] ; box.yB =                  p000[1];
    box.yA = std::min(box.yA, p001[1]); box.yB = std::max(box.yB, p001[1]);
    box.yA = std::min(box.yA, p010[1]); box.yB = std::max(box.yB, p010[1]);
    box.yA = std::min(box.yA, p011[1]); box.yB = std::max(box.yB, p011[1]);
    box.yA = std::min(box.yA, p100[1]); box.yB = std::max(box.yB, p100[1]);
    box.yA = std::min(box.yA, p101[1]); box.yB = std::max(box.yB, p101[1]);
    box.yA = std::min(box.yA, p110[1]); box.yB = std::max(box.yB, p110[1]);
    box.yA = std::min(box.yA, p111[1]); box.yB = std::max(box.yB, p111[1]);

    box.zA =                  p000[2] ; box.zB =                  p000[2];
    box.zA = std::min(box.zA, p001[2]); box.zB = std::max(box.zB, p001[2]);
    box.zA = std::min(box.zA, p010[2]); box.zB = std::max(box.zB, p010[2]);
    box.zA = std::min(box.zA, p011[2]); box.zB = std::max(box.zB, p011[2]);
    box.zA = std::min(box.zA, p100[2]); box.zB = std::max(box.zB, p100[2]);
    box.zA = std::min(box.zA, p101[2]); box.zB = std::max(box.zB, p101[2]);
    box.zA = std::min(box.zA, p110[2]); box.zB = std::max(box.zB, p110[2]);
    box.zA = std::min(box.zA, p111[2]); box.zB = std::max(box.zB, p111[2]);
}

void ModBox::translate(glm::vec3 _pos) {
    xO += _pos.x; yO += _pos.y; zO += _pos.z;
    p000 = {p000[0]+_pos.x, p000[1]+_pos.y, p000[2]+_pos.z};
    p001 = {p001[0]+_pos.x, p001[1]+_pos.y, p001[2]+_pos.z};
    p010 = {p010[0]+_pos.x, p010[1]+_pos.y, p010[2]+_pos.z};
    p011 = {p011[0]+_pos.x, p011[1]+_pos.y, p011[2]+_pos.z};
    p100 = {p100[0]+_pos.x, p100[1]+_pos.y, p100[2]+_pos.z};
    p101 = {p101[0]+_pos.x, p101[1]+_pos.y, p101[2]+_pos.z};
    p110 = {p110[0]+_pos.x, p110[1]+_pos.y, p110[2]+_pos.z};
    p111 = {p111[0]+_pos.x, p111[1]+_pos.y, p111[2]+_pos.z};
}

bool ModBox::intersect_pnt(glm::vec3 _pos) {
    if (_pos.x < box.xA) return false;
    if (_pos.x > box.xB) return false;
    if (_pos.y < box.yA) return false;
    if (_pos.y > box.yB) return false;
    if (_pos.z < box.zA) return false;
    if (_pos.z > box.zB) return false;
    return true;
}

bool ModBox::intersect_box(const Box& _box) {
    if (_box.xB < box.xA) return false;
    if (_box.xA > box.xB) return false;
    if (_box.yB < box.yA) return false;
    if (_box.yA > box.yB) return false;
    if (_box.zB < box.zA) return false;
    if (_box.zA > box.zB) return false;
    return true;
}

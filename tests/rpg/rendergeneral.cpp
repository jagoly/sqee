#include <iostream>
#include <vector>

#include "rendergeneral.hpp"

using namespace sqt;

void SkyLight::update(glm::vec3 _dir, glm::vec3 _min, glm::vec3 _max) {
    _dir = glm::normalize(_dir);
    glm::vec3 centre = (_min + _max) / 2.f;
    viewMat = glm::lookAt(centre, centre + _dir, glm::vec3(0,1,0));

    glm::vec3 min = _min;
    glm::vec3 max = _max;

    glm::vec4 t;
    array<glm::vec3, 8> arr;

    t = viewMat * glm::vec4(min.x, min.y, max.z, 1);
    arr[0] = {t.x, t.y, t.z};
    t = viewMat * glm::vec4(min.x, min.y, min.z, 1);
    arr[1] = {t.x, t.y, t.z};
    t = viewMat * glm::vec4(min.x, max.y, min.z, 1);
    arr[2] = {t.x, t.y, t.z};
    t = viewMat * glm::vec4(min.x, max.y, max.z, 1);
    arr[3] = {t.x, t.y, t.z};

    t = viewMat * glm::vec4(max.x, min.y, max.z, 1);
    arr[4] = {t.x, t.y, t.z};
    t = viewMat * glm::vec4(max.x, min.y, min.z, 1);
    arr[5] = {t.x, t.y, t.z};
    t = viewMat * glm::vec4(max.x, max.y, min.z, 1);
    arr[6] = {t.x, t.y, t.z};
    t = viewMat * glm::vec4(max.x, max.y, max.z, 1);
    arr[7] = {t.x, t.y, t.z};

    float minX=centre.x, maxX=centre.x, minY=centre.y, maxY=centre.y, minZ=centre.z, maxZ=centre.z;
    for (glm::vec3& a : arr) {
        minX = glm::min(minX, a.x);
        maxX = glm::max(maxX, a.x);
        minY = glm::min(minY, a.y);
        maxY = glm::max(maxY, a.y);
        minZ = glm::min(minZ, a.z);
        maxZ = glm::max(maxZ, a.z);
    }
    projMat = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

    projViewMat = projMat * viewMat;
}

#include <iostream>

#include "rendergeneral.hpp"

using namespace sqt;

void SkyLight::update(glm::vec3 _dir, glm::uvec2 _mapSize, float _zBound) {
    //glm::vec3 invDir = glm::normalize(_dir * -1.f);
    _dir = glm::normalize(_dir);
    glm::vec3 centre = {_mapSize.x / 2.f, _mapSize.y / 2.f, _zBound / 2.f};
    viewMat = glm::lookAt(centre, centre + _dir, glm::vec3(0,1,0));

    float xM = _mapSize.x / 2.f + 1;
    float yM = _mapSize.y / 2.f + 1;
    float zM = _zBound / 2.f + 1;

    glm::vec4 t;
    std::vector<glm::vec3> vec(8);

    t = viewMat * glm::vec4(-xM, -yM, zM, 0);
    vec[0] = {t.x, t.y, t.z};
    t = viewMat * glm::vec4(-xM, -yM, -zM, 0);
    vec[1] = {t.x, t.y, t.z};
    t = viewMat * glm::vec4(-xM, yM, -zM, 0);
    vec[2] = {t.x, t.y, t.z};
    t = viewMat * glm::vec4(-xM, yM, zM, 0);
    vec[3] = {t.x, t.y, t.z};

    t = viewMat * glm::vec4(xM, -yM, zM, 0);
    vec[4] = {t.x, t.y, t.z};
    t = viewMat * glm::vec4(xM, -yM, -zM, 0);
    vec[5] = {t.x, t.y, t.z};
    t = viewMat * glm::vec4(xM, yM, -zM, 0);
    vec[6] = {t.x, t.y, t.z};
    t = viewMat * glm::vec4(xM, yM, zM, 0);
    vec[7] = {t.x, t.y, t.z};

    float minX=centre.x, maxX=centre.x, minY=centre.y, maxY=centre.y, minZ=centre.z, maxZ=centre.z;

    for (glm::vec3& v : vec) {
        minX = glm::min(minX, v.x);
        maxX = glm::max(maxX, v.x);
        minY = glm::min(minY, v.y);
        maxY = glm::max(maxY, v.y);
        minZ = glm::min(minZ, v.z);
        maxZ = glm::max(maxZ, v.z);
    }

    projMat = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

    projViewMat = projMat * viewMat;
}

#include "rendergeneral.hpp"

using namespace sqt;

void SkyLight::update(glm::vec3 _dir) {
    glm::vec3 invDir = glm::normalize(_dir * -1.f);

    viewMat = glm::lookAt(invDir, glm::vec3(0,0,0), glm::vec3(0,1,0));
    //projMat = glm::ortho(-1.f, 65.f, -1.f, 65.f, -65.f, 1.f);
    projMat = glm::ortho(-1.f, 17.f, -1.f, 17.f, -17.f, 1.f);

    projViewMat = projMat * viewMat;
}

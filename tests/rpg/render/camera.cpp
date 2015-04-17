#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <sqee/gl/uniformbuffers.hpp>

#include "camera.hpp"

using namespace sqt;

MainCamera::MainCamera() : UboCamera({{"splits", 4}}) {}

void MainCamera::update() {
    sq::UboCamera::update();
    for (int i = 0; i < 4; i++)
        ubo->update("splits", &(csmArr[i].first), i, 1);
}

void MainCamera::recalc_frustums() {
    const float weight = 0.666f;
    float prevSplit = range.x;
    for (uint sliceInd = 0; sliceInd < 4; sliceInd++) {
        float f = float(sliceInd+1) / 4.f;
        float splitUni = range.x + (range.y - range.x) * f;
        float splitLog = range.x * glm::pow(range.y / range.x, f);
        float splitMix = glm::mix(splitUni, splitLog, weight);
        mat4 proj = glm::perspective(fov, size.x/size.y, prevSplit, splitMix);
        csmArr[sliceInd].first = splitMix;
        csmArr[sliceInd].second = sq::make_Frustum(glm::inverse(proj * viewMat));
        prevSplit = splitMix;
    }
}

#include <glm/gtx/string_cast.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "gl/gl_ext_3_3.hpp"
#include "app/logging.hpp"
#include "gl/cameras.hpp"

using namespace sq;

Camera::Camera(bool _useUBO) : useUBO(_useUBO) {
    if (useUBO) {
        ubo.reserve("proj", 16);
        ubo.reserve("view", 16);
        ubo.reserve("pos", 3);
        ubo.reserve("near", 1);
        ubo.reserve("dir", 3);
        ubo.reserve("far", 1);
        ubo.reserve("csmSplitsA", 6);
        ubo.reserve("csmSplitsB", 4);
        ubo.reserve("csmCntA", 1);
        ubo.reserve("csmCntB", 1);
        ubo.create();
    }
}

void Camera::recalc_frustums() {
    const float weight = 0.666f;
    const float blend = 0.f;

    float prevSplit = range.x;
    float prevPrevSplit = range.x;
    for (uint sliceInd = 0; sliceInd < csmVecA.size(); sliceInd++) {
        float f = float(sliceInd+1) / csmVecA.size();
        float splitUni = range.x + (range.y - range.x) * f;
        float splitLog = range.x * pow(range.y / range.x, f);
        float splitMix = glm::mix(splitUni, splitLog, weight);
        float splitBeg = glm::mix(prevSplit, prevPrevSplit, blend);
        glm::mat4 proj = make_persMat({splitBeg, splitMix}, size, fov);

        csmVecA[sliceInd].first = splitMix;
        csmVecA[sliceInd].second = make_worldFrustum(glm::inverse(proj * viewMat));
        prevPrevSplit = prevSplit;
        prevSplit = splitMix;
    }

    for (uint sliceInd = 0; sliceInd < csmVecB.size(); sliceInd++) {
        float f = float(sliceInd+1) / csmVecB.size();
        float splitUni = range.x + (range.y - range.x) * f;
        float splitLog = range.x * pow(range.y / range.x, f);
        float splitMix = glm::mix(splitUni, splitLog, weight);
        csmVecB[sliceInd].first = splitMix;
        glm::mat4 proj = make_persMat({range.x, splitMix}, size, fov);
        csmVecB[sliceInd].second = make_worldFrustum(glm::inverse(proj * viewMat));
    }
}

void Camera::update() {
    viewMat = make_viewMat(pos, dir);
    projMat = make_persMat(range, size, fov);

    if (useUBO) {
        uint cntA = csmVecA.size();
        uint cntB = csmVecB.size();

        ubo.bind(0);
        ubo.update("proj", &projMat);
        ubo.update("view", &viewMat);
        ubo.update("pos", &pos);
        ubo.update("near", &range.x);
        ubo.update("dir", &dir);
        ubo.update("far", &range.y);
        for (uint i = 0; i < cntA; i++)
            ubo.update("csmSplitsA", &(csmVecA[i].first), i, 1);
        for (uint i = 0; i < cntB; i++)
            ubo.update("csmSplitsB", &(csmVecB[i].first), i, 1);
        ubo.update("csmCntA", &cntA);
        ubo.update("csmCntB", &cntB);
    }
}

#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <sqee/gl/gl_ext_3_3.hpp>
#include <sqee/app/logging.hpp>


#include "light.hpp"

using namespace sqt;

////////////////
/// SkyLight ///
////////////////

SkyLight::SkyLight() {
    ubo.reserve("colour", 4);
    ubo.reserve("direction", 4);
    ubo.reserve("matArr", 4*16);
    ubo.create();
}

void SkyLight::update() {
    vec3 t1(glm::cross(direction, {1, 0, 0})), t2(glm::cross(direction, {0, 1, 0}));
    vec3 tangent = glm::normalize(glm::length(t1) > glm::length(t2) ? t1 : t2);
    for (int i = 0; i < 4; i++) {
        const auto& csm = camera->csmArr[i];
        vec3 frusCentre = sq::calc_frusCentre(csm.second);
        mat4 viewMat = glm::lookAt(frusCentre-direction, frusCentre, tangent);
        vec3 arr[8] {
            vec3(viewMat * vec4(csm.second.xyz, 1)),
            vec3(viewMat * vec4(csm.second.xyZ, 1)),
            vec3(viewMat * vec4(csm.second.xYz, 1)),
            vec3(viewMat * vec4(csm.second.xYZ, 1)),
            vec3(viewMat * vec4(csm.second.Xyz, 1)),
            vec3(viewMat * vec4(csm.second.XyZ, 1)),
            vec3(viewMat * vec4(csm.second.XYz, 1)),
            vec3(viewMat * vec4(csm.second.XYZ, 1))
        };

        vec3 minO = {INFINITY, INFINITY, INFINITY};
        vec3 maxO = {-INFINITY, -INFINITY, -INFINITY};
        for (auto& vec : arr) {
            minO = glm::min(minO, vec);
            maxO = glm::max(maxO, vec);
        }
        minO.z = glm::min(minO.z, glm::min(minO.x, minO.y));
        maxO.z = glm::max(maxO.z, glm::max(maxO.x, maxO.y));

        mat4 projMat = glm::ortho(minO.x, maxO.x, minO.y, maxO.y, minO.z, maxO.z);
        matArr[i] = projMat * viewMat;
    }
}

void SkyLight::bind_ubo() {
    ubo.bind(1);
    ubo.update("colour", &colour);
    ubo.update("direction", &direction);
    ubo.update("matArr", matArr.data());
}


/////////////////
/// SpotLight ///
/////////////////

SpotLight::SpotLight() {
    ubo.reserve("position", 3);
    ubo.reserve("angle", 1);
    ubo.reserve("direction", 3);
    ubo.reserve("intensity", 1);
    ubo.reserve("colour", 3);
    ubo.reserve("softness", 1);
    ubo.reserve("matrix", 16);
    ubo.create();
}

void SpotLight::update() {
    vec3 t1(glm::cross(direction, {1, 0, 0})), t2(glm::cross(direction, {0, 1, 0}));
    vec3 tangent = glm::normalize(glm::length(t1) > glm::length(t2) ? t1 : t2);
    mat4 viewMat = glm::lookAt(position, position+direction, tangent);
    mat4 projMat = glm::perspective(angle * 2.f, 1.f, 1.5f, intensity);
    matrix = projMat * viewMat;
//    frustum = sq::make_Frustum(glm::inverse(matrix));

//    array<vec2, 8> points = {
//        vec2(camera->viewMat * vec4(frustum.xyz, 1)),
//        vec2(camera->viewMat * vec4(frustum.xyZ, 1)),
//        vec2(camera->viewMat * vec4(frustum.xYz, 1)),
//        vec2(camera->viewMat * vec4(frustum.xYZ, 1)),
//        vec2(camera->viewMat * vec4(frustum.Xyz, 1)),
//        vec2(camera->viewMat * vec4(frustum.XyZ, 1)),
//        vec2(camera->viewMat * vec4(frustum.XYz, 1)),
//        vec2(camera->viewMat * vec4(frustum.XYZ, 1)),
//    };

//    vec2 minX = {INFINITY,0}; vec2 maxX = {-INFINITY,0};
//    vec2 minY = {0,INFINITY}; vec2 maxY = {0,-INFINITY};
//    int minXind, maxXind, minYind, maxYind;
//    for (int i = 0; i < 8; i++) {
//        if (points[i].x < minX.x) { minX = points[i]; minXind = i; }
//        if (points[i].x > maxX.x) { maxX = points[i]; maxXind = i; }
//        if (points[i].y < minY.y) { minY = points[i]; minYind = i; }
//        if (points[i].y > maxY.y) { maxY = points[i]; maxYind = i; }
//    }

//    shape = {minX, maxX, minY, maxY};
//    for (int i = 0; i < 8; i++)
//        if (i != minXind && i != maxXind && i != minYind && i != maxYind &&
//            !sq::point_in_tri(points[i], minX, maxY, maxX) &&
//            !sq::point_in_tri(points[i], maxX, minY, minX))
//            shape.push_back(points[i]);

//    sq::log_only("\nUnsorted:");
//    for (auto& vec : shape) sq::log_only(glm::to_string(vec));

//    for (uint i = 0; i+1 < shape.size(); i++) {
//        float dist = glm::distance(shape[i], shape[i+1]);
//        for (uint j = i+2; j+1 < shape.size(); j++) {
//            float newDist = glm::distance(shape[i], shape[j]);
//            if (newDist < dist) {
//                dist = newDist;
//                std::swap(shape[i+1], shape[j]);
//            }
//        }
//    }

//    for (uint i = 0; i < shape.size(); i++) {
//        shape[i] = vec2(camera->projMat * vec4(shape[i], 0, 1));
//    }

//    sq::log_only("Sorted:");
//    for (auto& vec : shape)
//        sq::log_only(glm::to_string(vec));
}

void SpotLight::bind_ubo() {
    ubo.bind(1);
    ubo.update("position", &position);
    ubo.update("angle", &angle);
    ubo.update("direction", &direction);
    ubo.update("intensity", &intensity);
    ubo.update("colour", &colour);
    ubo.update("softness", &softness);
    ubo.update("matrix", &matrix);
}

void SpotLight::draw() {
    static GLuint vao;
    static GLuint vbo;
    static bool first = true;
    if (first) { first = false;
        gl::GenBuffers(1, &vbo);
        gl::GenVertexArrays(1, &vao);
    }
    gl::BindBuffer(gl::ARRAY_BUFFER, vbo);
    gl::BufferData(gl::ARRAY_BUFFER, shape.size() * 2 * sizeof(float), &shape, gl::DYNAMIC_DRAW);
    gl::BindVertexArray(vao);
    gl::EnableVertexAttribArray(0);
    gl::VertexAttribPointer(0, 2, gl::FLOAT, false, 0, nullptr);
    gl::DrawArrays(gl::TRIANGLE_STRIP, 0, shape.size());
}

#version 330

layout(location = 0) in vec3 VC_pos;
layout(location = 4) in vec3 VN_pos;

uniform int anim;
uniform float percent;
uniform mat4 projMat, viewMat, modelMat;

void main() {
    vec3 M_pos;

    if (anim == 0) {
        M_pos = VC_pos;

    } else {
        M_pos = mix(VC_pos, VN_pos, percent);
    }

    gl_Position = projMat * viewMat * modelMat * vec4(M_pos, 1.f);
}

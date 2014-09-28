#version 330

layout(location = 0) in vec3 VC_pos;
layout(location = 1) in vec3 VC_norm;
layout(location = 2) in vec2 VC_texcoord;
layout(location = 3) in vec3 VC_tangent;
layout(location = 4) in vec3 VN_pos;
layout(location = 5) in vec3 VN_norm;
layout(location = 6) in vec2 VN_texcoord;
layout(location = 7) in vec3 VN_tangent;

uniform int anim;
uniform float percent;
uniform mat4 projMat, viewMat, modelMat;
uniform mat4 shadProjViewMat;

out vec3 v_pos;
out vec3 shadcoord;
out vec2 texcoord;
out vec3 n, t, b;

void main() {
    mat3 normMat = mat3(transpose(inverse(viewMat * modelMat)));
    vec3 M_pos;

    if (anim == 0) {
        v_pos = vec4(viewMat * modelMat * vec4(VC_pos, 1.f)).xyz;
        shadcoord = vec4(shadProjViewMat * modelMat * vec4(VC_pos, 1.f)).xyz;
        texcoord = VC_texcoord;
        n = normalize(normMat * VC_norm);
        t = normalize(normMat * (VC_tangent * -1));
        b = normalize(normMat * (cross(VC_norm, VC_tangent) * -1));

        M_pos = VC_pos;

    } else {
        M_pos = mix(VC_pos, VN_pos, percent);
        vec3 M_norm = mix(VC_norm, VN_norm, percent);
        vec3 M_tangent = mix(VC_tangent, VN_tangent, percent);

        v_pos = vec4(viewMat * modelMat * vec4(M_pos, 1.f)).xyz;
        shadcoord = vec4(shadProjViewMat * modelMat * vec4(M_pos, 1.f)).xyz;
        texcoord = mix(VC_texcoord, VN_texcoord, percent);
        n = normalize(normMat * M_norm);
        t = normalize(normMat * (M_tangent * -1));
        b = normalize(normMat * (cross(M_norm, M_tangent) * -1));
    }

    gl_Position = projMat * viewMat * modelMat * vec4(M_pos, 1.f);
}

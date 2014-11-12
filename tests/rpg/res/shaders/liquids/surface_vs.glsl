#version 330
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 V_pos;
layout(location = 1) in vec2 V_texcoord;

layout(std140, binding = 0) uniform cameraBlock {
    mat4 projMat, viewMat;
    vec3 camPos, camRot;
    vec2 zRange;
};

// From Other
uniform mat4 shadProjViewMat;

// From Object
uniform vec3 translation;
uniform vec2 flowOffset;
uniform float scale;

out vec3 w_pos;
out vec3 v_pos;
out vec4 p_pos;
out vec3 shadcoord;
out vec2 texcoord;
out vec3 n, t, b;


void main() {
    shadcoord = vec4(shadProjViewMat * vec4(V_pos + translation, 1)).xyz / 2.f + 0.5f;
    texcoord = V_texcoord - flowOffset / scale;

    mat3 normMat = mat3(transpose(inverse(viewMat)));
    n = normalize(normMat * vec3(0, 0, 1));
    t = normalize(normMat * vec3(0, 1, 0));
    b = normalize(normMat * vec3(1, 0, 0));

    w_pos = V_pos + translation;
    v_pos = vec4(viewMat * vec4(w_pos, 1)).xyz;
    p_pos = projMat * vec4(v_pos, 1);
    gl_Position = p_pos;
}

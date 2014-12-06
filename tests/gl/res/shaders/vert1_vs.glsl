#version 330
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 V_pos;
layout(location = 1) in vec3 V_norm;
layout(location = 2) in vec3 V_tan;
layout(location = 3) in vec2 V_texcoord;

uniform mat4 viewMat, projMat;
uniform vec3 translation;
uniform mat3 normMat;

out vec3 w_pos, v_pos;
out vec3 N, T, B;
out vec2 texcoord;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    texcoord = V_texcoord;

    N = normalize(normMat * V_norm);
    T = normalize(normMat * V_tan);
    B = normalize(normMat * -cross(V_norm, V_tan));

    w_pos = V_pos + translation;
    v_pos = vec4(viewMat * vec4(w_pos, 1)).xyz;

    gl_Position = projMat * vec4(v_pos, 1);
}

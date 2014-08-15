#version 330

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_norm;
layout(location = 2) in vec2 v_texcoord;
layout(location = 3) in vec4 v_tangent;

uniform mat4 projMat, viewMat, modelMat;

out vec2 texcoord;
out vec3 n, t, b;

void main() {
    texcoord = v_texcoord;
    mat4 normMat = transpose(inverse(viewMat * modelMat));
    n = normalize(vec4(normMat * vec4(v_norm, 0.f)).xyz);
    t = normalize(vec4(normMat * v_tangent)).xyz;
    b = normalize(cross(v_norm, v_tangent.xyz) * v_tangent.w);

    gl_Position = projMat * viewMat * modelMat * vec4(v_pos, 1.f);
}

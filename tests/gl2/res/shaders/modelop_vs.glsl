#version 330

layout(location = 0) in vec3 V_pos;
layout(location = 1) in vec3 V_norm;
layout(location = 2) in vec2 V_texcoord;
layout(location = 3) in vec3 V_tangent;

uniform mat4 projMat, viewMat, modelMat;
uniform mat4 shadProjViewMat;

out vec3 v_pos;
out vec3 shadcoord;
out vec2 texcoord;
out vec3 n, t, b;

void main() {
    texcoord = V_texcoord;

    mat3 normMat = mat3(transpose(inverse(viewMat * modelMat)));
    n = normalize(normMat * V_norm);
    t = normalize(normMat * (V_tangent * -1));
    b = normalize(normMat * (cross(V_norm, V_tangent) * -1));

    v_pos = vec4(viewMat * modelMat * vec4(V_pos, 1.f)).xyz;
    shadcoord = vec4(shadProjViewMat * modelMat * vec4(V_pos, 1.f)).xyz;
    gl_Position = projMat * viewMat * modelMat * vec4(V_pos, 1.f);
}

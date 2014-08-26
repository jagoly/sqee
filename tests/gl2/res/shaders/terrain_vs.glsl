#version 330

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_norm;
layout(location = 2) in vec3 v_texcoord;
layout(location = 3) in vec3 v_tangent;

uniform mat4 projMat, viewMat;
uniform ivec2 mapSize;

out vec3 texcoord;
out vec2 mapPos;
out vec3 n, t, b;

void main() {
    texcoord = v_texcoord;
    mapPos.x = v_pos.x / mapSize.x;
    mapPos.y = v_pos.y / mapSize.y;

    mat3 normMat = mat3(transpose(inverse(viewMat)));
    n = normalize(normMat * v_norm);
    t = normalize(normMat * v_tangent);
    b = normalize(normMat * cross(v_norm, v_tangent));

    gl_Position = projMat * viewMat * vec4(v_pos, 1.f);
}

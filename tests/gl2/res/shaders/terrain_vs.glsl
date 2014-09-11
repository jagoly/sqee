#version 330

layout(location = 0) in vec3 V_pos;
layout(location = 1) in vec3 V_norm;
layout(location = 2) in vec3 V_texcoord;
layout(location = 3) in vec3 V_tangent;

uniform int shadQuality;

uniform mat4 projMat, viewMat;
uniform mat4 shadProjViewMat;
uniform uvec2 mapSize;

out vec3 v_pos;
out vec3 shadcoord;
out vec3 texcoord;
out vec2 mapPos;
out vec3 n, t, b;

void main() {
    texcoord = V_texcoord;
    mapPos.x = V_pos.x / mapSize.x;
    mapPos.y = V_pos.y / mapSize.y;

    mat3 normMat = mat3(transpose(inverse(viewMat)));
    n = normalize(normMat * V_norm);
    t = normalize(normMat * V_tangent);
    b = normalize(normMat * cross(V_norm, V_tangent));

    v_pos = vec4(viewMat * vec4(V_pos, 1.f)).xyz;

    if (shadQuality > 0)
        shadcoord = vec4(shadProjViewMat * vec4(V_pos, 1.f)).xyz;
    gl_Position = projMat * viewMat * vec4(V_pos, 1.f);
}

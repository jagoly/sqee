#version 330

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_norm;
layout(location = 2) in vec2 v_texCoord;
layout(location = 3) in vec4 v_tangent;

uniform mat4 modelMatrix, projMatrix, viewMatrix;
uniform vec3 w_camPos, w_lightDir;

out vec2 texCoord;
out vec3 t_viewDir, t_lightDir;

void main() {
    texCoord = v_texCoord;

    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(v_pos, 1.f);

    vec3 bitangent = cross(v_norm, v_tangent.xyz) * v_tangent.w;
    vec3 w_viewDir = normalize(w_camPos - v_pos);

    t_viewDir = vec3(
        dot(v_tangent.xyz, w_viewDir),
        dot(bitangent, w_viewDir),
        dot(v_norm, w_viewDir)
    );

    t_lightDir = vec3(
        dot(v_tangent.xyz, w_lightDir),
        dot(bitangent, w_lightDir),
        dot(v_norm, w_lightDir)
    );
}

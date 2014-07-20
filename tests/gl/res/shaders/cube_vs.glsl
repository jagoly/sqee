#version 330

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_texCoords;
layout(location = 2) in vec3 vertex_normal;

uniform mat4 projMatrix, viewMatrix, transformMatrix;

out vec3 posEye, normalEye;
out vec2 texCoords;

void main() {
    texCoords = vertex_texCoords;
    posEye = vec3(viewMatrix * transformMatrix * vec4(vertex_position, 1.f));
    normalEye = vec3(viewMatrix * transformMatrix * vec4(vertex_normal, 0.f));

    //gl_Position = projMatrix * viewMatrix * transformMatrix * vec4(vertex_position, 1.f);
    gl_Position = projMatrix * vec4(posEye, 1.f);
}

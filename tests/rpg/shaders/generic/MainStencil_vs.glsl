// GLSL Vertex Shader

layout(location=0) in vec3 V_pos;

#include headers/blocks/Camera

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };

uniform mat4 matrix;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    gl_Position = CB.proj * CB.view * matrix * vec4(V_pos, 1.f);
}

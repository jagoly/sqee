// GLSL Vertex Shader

layout(location=0) in vec3 V_pos;

#include builtin/blocks/camera
#include headers/blocks/reflector

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=2) uniform REFLECTORBLOCK { ReflectorBlock RB; };

uniform mat4 matrix;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    vec3 w_pos = vec3(matrix * vec4(V_pos, 1.f)) - RB.trans;
    gl_Position = CB.proj * CB.view * vec4(reflect(w_pos, RB.normal) + RB.trans, 1.f);
}

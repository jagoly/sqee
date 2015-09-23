// GLSL Vertex Shader

layout(location=0) in vec3 V_pos;

#include builtin/blocks/camera

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };

uniform vec3 trans;

out vec3 o_pos;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    o_pos = normalize(V_pos);
//    o_pos = V_pos;
    gl_Position = CB.proj * CB.view * vec4(V_pos * 0.5f + trans, 1.f);
}

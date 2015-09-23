// GLSL Vertex Shader

layout(location=0) in vec3 V_pos;
layout(location=1) in vec3 V_norm;
layout(location=2) in vec4 V_tan;
layout(location=3) in vec2 V_texcrd;

#include builtin/blocks/camera
#include headers/blocks/msimple

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform MSIMPLEBLOCK { MSimpleBlock MB; };

out vec2 texcrd;
out vec3 N, T, B;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    texcrd = V_texcrd;
    N = normalize(mat3(MB.normMat) * V_norm);
    T = normalize(mat3(MB.normMat) * -V_tan.xyz);
    B = normalize(mat3(MB.normMat) * -cross(V_norm, V_tan.xyz) * V_tan.w);
    gl_Position = CB.proj * CB.view * MB.matrix * vec4(V_pos, 1.f);
}

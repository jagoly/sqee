// GLSL Vertex Shader

layout(location=0) in vec3 V_pos;
layout(location=1) in vec3 V_norm;
layout(location=3) in vec2 V_texcrd;

#include builtin/blocks/camera
#include headers/blocks/msimple
#include headers/blocks/reflect

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform MSIMPLEBLOCK { MSimpleBlock MB; };
layout(std140, binding=2) uniform REFLECTBLOCK { ReflectBlock RB; };

out vec2 texcrd;
out vec3 surface;

out gl_PerVertex {
    vec4 gl_Position;
    float gl_ClipDistance[1];
};


void main() {
    texcrd = V_texcrd;
    surface = mat3(MB.normMat) * normalize(V_norm);
    vec3 w_pos = vec3(MB.matrix * vec4(V_pos, 1.f)) - RB.trans;
    gl_Position = CB.proj * CB.view * vec4(reflect(w_pos, RB.normal) + RB.trans, 1.f);
    gl_ClipDistance[0] = dot(w_pos, RB.normal);
}

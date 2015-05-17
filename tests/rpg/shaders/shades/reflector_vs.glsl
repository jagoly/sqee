#version 330
#extension GL_ARB_shading_language_420pack : enable

// define REFLECT

#include "headers/blocks/camera"
#include "headers/blocks/reflector"

layout(location=0) in vec3 V_pos;
layout(location=3) in vec2 V_texcrd;

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=2) uniform REFLECTORBLOCK { ReflectorBlock RB; };

out vec2 texcrd;
out vec3 v_norm, v_from;
out noperspective vec2 s_tcrd;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    texcrd = V_texcrd;
    v_norm = normalize(mat3(CB.view) * RB.normal);
    v_from = -normalize(vec3(CB.view * RB.matrix * vec4(V_pos, 1.f)));
    gl_Position = CB.proj * CB.view * RB.matrix * vec4(V_pos, 1.f);
    s_tcrd = gl_Position.xy / gl_Position.w * 0.5f + 0.5f;
}

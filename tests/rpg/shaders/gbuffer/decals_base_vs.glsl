#version 330
#extension GL_ARB_shading_language_420pack : enable

layout(location=0) in vec3 V_pos;

#include "headers/blocks/camera"
#include "headers/blocks/decal"

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform DECALBLOCK { DecalBlock DB; };

out noperspective vec2 s_tcrd;
out vec3 viewNorm, viewTan;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    vec3 w_pos = vec3(DB.matrix * vec4(V_pos, 1.f));
    gl_Position = CB.proj * CB.view * vec4(w_pos, 1.f);
    s_tcrd = gl_Position.xy / gl_Position.w * 0.5f + 0.5f;
    viewNorm = mat3(CB.view) * mat3(DB.matrix) * vec3(0.f, 0.f, 1.f);
    viewTan = mat3(CB.view) * mat3(DB.matrix) * vec3(1.f, 0.f, 0.f);
}

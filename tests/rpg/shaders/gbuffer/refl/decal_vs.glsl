// GLSL Vertex Shader

layout(location=0) in vec3 V_pos;

#include builtin/blocks/camera
#include headers/blocks/decal
#include headers/blocks/reflect

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform DECALBLOCK { DecalBlock DB; };
layout(std140, binding=2) uniform REFLECTBLOCK { ReflectBlock RB; };

out noperspective vec2 s_tcrd;
out vec3 viewNorm;

out gl_PerVertex {
    vec4 gl_Position;
    float gl_ClipDistance[1];
};


void main() {
    vec3 w_pos = vec3(DB.matrix * vec4(V_pos, 1.f)) - RB.trans;
    gl_Position = CB.proj * CB.view * vec4(reflect(w_pos, RB.normal) + RB.trans, 1.f);
    s_tcrd = gl_Position.xy / gl_Position.w * 0.5f + 0.5f;
    viewNorm = mat3(CB.view) * mat3(DB.matrix) * vec3(0.f, 0.f, 1.f);
    gl_ClipDistance[0] = dot(w_pos, RB.normal);
}

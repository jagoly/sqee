// GLSL Vertex Shader

layout(location=0) in vec3 V_pos;
layout(location=5) in ivec4 V_bones;
layout(location=6) in vec4 V_weights;

uniform mat4 matrix;

#include headers/blocks/Skeleton

layout(std140, binding=1) uniform SKELETONBLOCK { SkeletonBlock SB; };

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {

    vec3 a_pos = vec3(0.f, 0.f, 0.f);

    if (V_bones.r != -1) a_pos += vec4(V_pos, 1.f) * SB.bones[V_bones.r] * V_weights.r;
    if (V_bones.g != -1) a_pos += vec4(V_pos, 1.f) * SB.bones[V_bones.g] * V_weights.g;
    if (V_bones.b != -1) a_pos += vec4(V_pos, 1.f) * SB.bones[V_bones.b] * V_weights.b;
    if (V_bones.a != -1) a_pos += vec4(V_pos, 1.f) * SB.bones[V_bones.a] * V_weights.a;

    gl_Position = matrix * vec4(a_pos, 1.f);
}

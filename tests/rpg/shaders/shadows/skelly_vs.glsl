#version 330
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 V_pos;
layout(location = 3) in vec2 V_texcrd;
layout(location = 4) in ivec4 V_bonesABCD;
layout(location = 5) in ivec4 V_bonesEFGH;
layout(location = 6) in vec4 V_weightsABCD;
layout(location = 7) in vec4 V_weightsEFGH;

#include "headers/blocks/mskelly"

layout(std140, binding=1) uniform MSKELLYBLOCK { MSkellyBlock MB; };

uniform mat4 matrix;

out vec2 texcrd;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    int bones[8] = {
        V_bonesABCD.r, V_bonesABCD.g, V_bonesABCD.b, V_bonesABCD.a,
        V_bonesEFGH.r, V_bonesEFGH.g, V_bonesEFGH.b, V_bonesEFGH.a
    };

    float weights[8] = {
        V_weightsABCD.r, V_weightsABCD.g, V_weightsABCD.b, V_weightsABCD.a,
        V_weightsEFGH.r, V_weightsEFGH.g, V_weightsEFGH.b, V_weightsEFGH.a
    };

    vec3 a_pos  = vec3(0.f, 0.f, 0.f);

    for (int i = 0; i < 8; i++) {
        int b = bones[i]; if (b == -1) break;
        a_pos  += vec4(V_pos, 1) * MB.bones[b] * weights[i];
    }

    texcrd = V_texcrd;
    gl_Position = matrix * vec4(a_pos, 1);
}

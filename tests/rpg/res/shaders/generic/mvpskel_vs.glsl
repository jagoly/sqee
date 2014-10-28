#version 330
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 V_pos;
layout(location = 3) in ivec4 V_bonesABCD;
layout(location = 4) in ivec4 V_bonesEFGH;
layout(location = 5) in vec4 V_weightsABCD;
layout(location = 6) in vec4 V_weightsEFGH;

uniform mat4 projMat, viewMat, modelMat;

uniform vec4[40] s_quat;
uniform vec3[40] s_offs;

void main() {
    int bones[8] = {
        V_bonesABCD.r, V_bonesABCD.g, V_bonesABCD.b, V_bonesABCD.a,
        V_bonesEFGH.r, V_bonesEFGH.g, V_bonesEFGH.b, V_bonesEFGH.a
    };

    float weights[8] = {
        V_weightsABCD.r, V_weightsABCD.g, V_weightsABCD.b, V_weightsABCD.a,
        V_weightsEFGH.r, V_weightsEFGH.g, V_weightsEFGH.b, V_weightsEFGH.a
    };

    vec3 a_pos = vec3(0);
    for (int i = 0; i < 8; i++) {
        int b = bones[i]; if (b < 0) break;
        float w = s_quat[b].r; float x = s_quat[b].g; float y = s_quat[b].b; float z = s_quat[b].a;
        mat4 bone = mat4(
            1-2*y*y-2*z*z,  2*x*y-2*w*z,    2*x*z+2*w*y,    s_offs[b].x,
            2*x*y+2*w*z,    1-2*x*x-2*z*z,  2*y*z-2*w*x,    s_offs[b].y,
            2*x*z-2*w*y,    2*y*z+2*w*x,    1-2*x*x-2*y*y,  s_offs[b].z,
            0, 0, 0, 1
        );
        bone = transpose(bone);

        a_pos += (bone * vec4(V_pos, 1)).xyz * weights[i];
    }

    gl_Position = projMat * viewMat * modelMat * vec4(a_pos, 1.f);
}

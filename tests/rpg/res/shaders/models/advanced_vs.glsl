#version 330
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 V_pos;
layout(location = 1) in vec3 V_norm;
layout(location = 2) in vec2 V_texcoord;
layout(location = 3) in ivec4 V_bonesABCD;
layout(location = 4) in ivec4 V_bonesEFGH;
layout(location = 5) in vec4 V_weightsABCD;
layout(location = 6) in vec4 V_weightsEFGH;

layout(std140, binding = 0) uniform cameraBlock {
    mat4 projMat, viewMat;
    vec3 camPos, camRot;
    vec2 zRange;
};

uniform mat4 reflMat;
uniform mat4 modelMat;
uniform mat4 shadProjViewMat;
uniform float clipZ;

uniform vec4[40] s_quat;
uniform vec3[40] s_offs;

out vec3 v_pos;
out vec3 shadcoord;
out vec2 texcoord;
out vec3 N, T, B;
out vec3 w_pos;

void main() {
    texcoord = V_texcoord;

    int bones[8] = {
        V_bonesABCD.r, V_bonesABCD.g, V_bonesABCD.b, V_bonesABCD.a,
        V_bonesEFGH.r, V_bonesEFGH.g, V_bonesEFGH.b, V_bonesEFGH.a
    };

    float weights[8] = {
        V_weightsABCD.r, V_weightsABCD.g, V_weightsABCD.b, V_weightsABCD.a,
        V_weightsEFGH.r, V_weightsEFGH.g, V_weightsEFGH.b, V_weightsEFGH.a
    };

    vec3 a_norm = vec3(0);
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

        a_norm += (bone * vec4(V_norm, 1)).xyz * weights[i];
        a_pos += (bone * vec4(V_pos, 1)).xyz * weights[i];
    }

    vec3 w_norm = a_norm;
    w_pos = vec4(modelMat * vec4(a_pos, 1)).xyz;
    shadcoord = vec4(shadProjViewMat * vec4(w_pos, 1)).xyz / 2.f + 0.5f;

    mat3 normMat = mat3(transpose(inverse(viewMat * modelMat)));
    vec3 tangent = normalize(cross(w_norm, vec3(0, 1, 0)));
    N = normalize(normMat * w_norm);
    T = normalize(normMat * -tangent);
    B = normalize(normMat * -cross(w_norm, tangent));

    v_pos = vec4(viewMat * vec4(w_pos, 1)).xyz;
    gl_ClipDistance[0] = w_pos.z - clipZ;
    gl_ClipDistance[1] = -(w_pos.z - clipZ);

    gl_Position = projMat * viewMat * vec4(vec4(reflMat * vec4(w_pos, 1)).xyz, 1);
}

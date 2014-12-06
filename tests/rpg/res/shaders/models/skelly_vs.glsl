#version 330
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 V_pos;
layout(location = 1) in vec3 V_norm;
layout(location = 2) in vec3 V_tan;
layout(location = 3) in vec2 V_texcoord;
layout(location = 4) in ivec4 V_bonesABCD;
layout(location = 5) in ivec4 V_bonesEFGH;
layout(location = 6) in vec4 V_weightsABCD;
layout(location = 7) in vec4 V_weightsEFGH;

layout(std140, binding = 0) uniform cameraBlock {
    mat4 projMat, viewMat;
    vec3 camPos; float zNear;
    vec3 camRot; float zFar;
};

layout(std140, binding = 1) uniform worldBlock {
    vec3 ambiColour;
    bool useSkylight;
    vec3 slDir;
    vec3 slColour;
    mat4 slShadMat;
    uint spCount;
    vec3 spPos[8];
    vec3 spDir[8];
    vec3 spColour[8];
    vec3 spFovIntSft[8];
    mat4 spShadMat[8];
};

uniform mat4 modelMat;
uniform mat3 normMat;
uniform vec4 skelQuat[40];
uniform vec3 skelOffs[40];
uniform float clipZ;
uniform bool useRefl;
uniform mat4 reflMat;

out vec3 w_pos, v_pos;
out vec3 N, T, B;
out vec2 texcoord;
out vec3 slShadcoord;
out vec3 spShadcoord[8];

out gl_PerVertex {
    vec4 gl_Position;
    float gl_ClipDistance[1];
};


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

    vec3 a_pos = vec3(0), a_norm = vec3(0), a_tan = vec3(0);
    for (int i = 0; i < 8; i++) {
        int b = bones[i]; if (b < 0) break;
        float w = skelQuat[b].r; float x = skelQuat[b].g;
        float y = skelQuat[b].b; float z = skelQuat[b].a;
        mat4 bone = transpose(mat4(
            1-2*y*y-2*z*z,  2*x*y-2*w*z,    2*x*z+2*w*y,    skelOffs[b].x,
            2*x*y+2*w*z,    1-2*x*x-2*z*z,  2*y*z-2*w*x,    skelOffs[b].y,
            2*x*z-2*w*y,    2*y*z+2*w*x,    1-2*x*x-2*y*y,  skelOffs[b].z,
            0, 0, 0, 1
        ));

        a_pos  += vec4(bone * vec4(V_pos, 1)).xyz * weights[i];
        a_norm += vec4(bone * vec4(V_norm, 1)).xyz * weights[i];
        a_tan  += vec4(bone * vec4(V_tan, 1)).xyz * weights[i];
    }

    w_pos = vec4(modelMat * vec4(a_pos, 1)).xyz;
    v_pos = vec4(viewMat * vec4(w_pos, 1)).xyz;
    slShadcoord = vec4(slShadMat * vec4(w_pos, 1)).xyz / 2.f + 0.5f;
    for (uint i = 0u; i < spCount; i++) {
        vec4 sc = spShadMat[i] * vec4(w_pos, 1);
        spShadcoord[i] = vec3(sc.xyz / sc.w) * 0.5f + 0.5f;
    }

    N = normalize(normMat * a_norm);
    T = normalize(normMat * a_tan);
    B = normalize(normMat * -cross(a_norm, a_tan));

    if (useRefl) {
        gl_ClipDistance[0] = w_pos.z - clipZ;
        gl_Position = projMat * viewMat * vec4(vec4(reflMat * vec4(w_pos, 1)).xyz, 1);
    } else {
        gl_ClipDistance[0] = -(w_pos.z - clipZ);
        gl_Position = projMat * vec4(v_pos, 1);
    }
}

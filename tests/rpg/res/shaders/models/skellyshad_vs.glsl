#version 330
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 V_pos;
layout(location = 4) in ivec4 V_bonesABCD;
layout(location = 5) in ivec4 V_bonesEFGH;
layout(location = 6) in vec4 V_weightsABCD;
layout(location = 7) in vec4 V_weightsEFGH;

uniform mat4 shadMat, modelMat;
uniform vec4 skelQuat[40];
uniform vec3 skelOffs[40];

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

    vec3 a_pos = vec3(0);
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

        a_pos += (bone * vec4(V_pos, 1)).xyz * weights[i];
    }

    gl_Position = shadMat * modelMat * vec4(a_pos, 1.f);
}

#version 330
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 V_pos;
layout(location = 1) in vec3 V_norm;
layout(location = 2) in vec2 V_texcoord;
layout(location = 3) in uvec4 V_bonesABCD;
layout(location = 4) in uvec4 V_bonesEFGH;
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

out vec3 v_pos;
out vec3 shadcoord;
out vec2 texcoord;
out vec3 n, t, b;
out vec3 w_pos;

void main() {
    mat3 normMat = mat3(transpose(inverse(viewMat * modelMat)));

    v_pos = vec4(viewMat * modelMat * vec4(V_pos, 1)).xyz;
    shadcoord = vec4(shadProjViewMat * modelMat * vec4(V_pos, 1)).xyz / 2.f + 0.5f;
    texcoord = V_texcoord;
    vec3 tangent = normalize(cross(V_norm, vec3(0, 1, 0)));
    n = normalize(normMat * V_norm);
    t = normalize(normMat * -tangent);
    b = normalize(normMat * -cross(V_norm, tangent));

    w_pos = vec4(modelMat * vec4(V_pos, 1)).xyz;
    gl_ClipDistance[0] = w_pos.z - clipZ;
    gl_ClipDistance[1] = -(w_pos.z - clipZ);

    gl_Position = projMat * viewMat * vec4(vec4(reflMat * vec4(w_pos, 1)).xyz, 1);
}

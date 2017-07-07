// GLSL Vertex Shader

layout(location=0) in vec3 V_pos;
layout(location=2) in vec3 V_norm;
layout(location=4) in vec3 V_colour;

#include uniform_block

layout(std140, binding=0) uniform UNIFORM { UniformBlock UB; };

layout(location=0) uniform mat4 modelMat;
layout(location=1) uniform mat3 normMat;

out vec3 v_pos;
out vec3 v_norm;
out vec3 colour;


void main() 
{
    v_pos = vec3(UB.cameraView * modelMat * vec4(V_pos, 1.f));
    v_norm = normalize(normMat * V_norm);
    colour = V_colour;

    gl_Position = UB.cameraProj * vec4(v_pos, 1.f);
}

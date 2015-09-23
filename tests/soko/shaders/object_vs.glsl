// GLSL Vertex Shader

layout(location=0) in vec3 V_pos;
layout(location=1) in vec3 V_norm;
layout(location=4) in vec3 V_colour;

#include builtin/blocks/camera
#include uniform_block

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform UNIFORMBLOCK { UniformBlock LB; };

uniform mat4 modelMat;
uniform mat3 normMat;

out vec3 v_pos;
out vec3 v_norm;
out vec3 colour;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    v_pos = vec3(CB.view * modelMat * vec4(V_pos, 1.f));
    gl_Position = CB.proj * vec4(v_pos, 1.f);
    v_norm = normalize(normMat * V_norm);
    colour = V_colour;
}

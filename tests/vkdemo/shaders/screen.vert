#version 450

struct CameraBlock
{
    mat4 model;
    mat4 view;
    mat4 proj;
};

layout(std140, binding=0) uniform CAMERA { CameraBlock CB; };

layout(location=0) in vec3 v_Position;
layout(location=1) in vec2 v_TexCoord;
layout(location=2) in vec3 v_Normal;
layout(location=3) in vec3 v_Tangent;

layout(location=0) out vec2 io_TexCoord;
layout(location=1) out vec3 io_Normal;

void main()
{
    io_TexCoord = v_TexCoord;
    io_Normal = v_Normal;
    gl_Position = CB.proj * CB.view * CB.model * vec4(v_Position, 1.f);
}

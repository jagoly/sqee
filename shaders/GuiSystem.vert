#version 450

layout(push_constant) uniform PushConstants { vec2 invWindowSize2; } PC;

layout(location=0) in vec2 v_Position;
layout(location=1) in vec2 v_TexCoord;
layout(location=2) in vec4 v_Colour;

layout(location=0) out vec2 io_TexCoord;
layout(location=1) out vec4 io_Colour;

out gl_PerVertex { vec4 gl_Position; };

void main()
{
    io_TexCoord = v_TexCoord;
    io_Colour = v_Colour;

    gl_Position = vec4(PC.invWindowSize2 * v_Position - 1.0, 0.0, 1.0);
}

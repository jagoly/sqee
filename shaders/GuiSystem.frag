#version 450

layout(binding=0) uniform sampler2D tex_Atlas;

layout(location=0) in vec2 io_TexCoord;
layout(location=1) in vec4 io_Colour;

layout(location=0) out vec4 frag_Colour;

void main()
{
    frag_Colour = io_Colour * texture(tex_Atlas, io_TexCoord);
}

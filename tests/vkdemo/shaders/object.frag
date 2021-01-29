#version 450

layout(set=1, binding=1) uniform sampler2D tx_Colour;

layout(location=0) in vec2 io_TexCoord;
layout(location=1) in vec3 io_Normal;

layout(location=0) out vec4 frag_Colour;

void main()
{
    //frag_Colour = vec4((io_Normal + 1.f) * 0.5f, 1.f);
    //frag_Colour *= texture(tx_Colour, io_TexCoord).rrra;
    frag_Colour = texture(tx_Colour, io_TexCoord).rgba;
    //frag_Colour = vec4(0.5,0.5,0.5,1.0);
}

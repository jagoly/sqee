#version 450

layout(set=0, binding=0) uniform sampler2D tx_Colour;

layout(location=0) in vec2 io_TexCoord;

layout(location=0) out vec4 frag_Colour;

void main()
{
    //frag_Colour = vec4((io_Normal + 1.f) * 0.5f, 1.f);
    //frag_Colour *= texture(tx_Colour, io_TexCoord).rrra;
    frag_Colour = texture(tx_Colour, io_TexCoord).rgba;
    //frag_Colour = vec4(0.5,0.5,0.5,1.0);
}

#version 450

layout(push_constant) uniform PC { mat4 pc_Matrix; };

layout(location=0) in vec2 v_Position;
layout(location=1) in vec2 v_TexCoord;
layout(location=2) in vec4 v_Colour;

layout(location=0) out vec2 io_TexCoord;
layout(location=1) out vec4 io_Colour;

//const vec2 positions[] =
//{
//  vec2(0,0), vec2(0,1), vec2(1,1),
//  vec2(0,0), vec2(1,0), vec2(1,1)
//};

void main()
{
    io_TexCoord = v_TexCoord;
    io_Colour = v_Colour;
    
//    gl_Position = vec4(positions[gl_VertexIndex % 6], 0.f, 1.f);

    gl_Position = pc_Matrix * vec4(v_Position, 0.f, 1.f);
}

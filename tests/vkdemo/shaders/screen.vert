#version 450

layout(location=0) out vec2 io_TexCoord;

void main()
{
    io_TexCoord = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    gl_Position = vec4(io_TexCoord * 2.f - 1.f, 0.f, 1.f);
}

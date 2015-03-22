#version 330
#extension GL_ARB_shading_language_420pack : enable

layout(location=0) in vec2 V_pos;

out vec2 texcrd;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    texcrd = V_pos * 0.5f + 0.5f;
    gl_Position = vec4(V_pos, 0.f, 1.f);
}

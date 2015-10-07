// GLSL Vertex Shader

#include builtin/data/screen

out vec2 texcrd;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    texcrd = V_pos * 0.5f + 0.5f;
    gl_Position = vec4(V_pos, 0.f, 1.f);
}

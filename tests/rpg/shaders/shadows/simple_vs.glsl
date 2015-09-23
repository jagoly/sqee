// GLSL Vertex Shader

layout(location=0) in vec3 V_pos;
layout(location=3) in vec2 V_texcrd;

uniform mat4 matrix;

out vec2 texcrd;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    texcrd = V_texcrd;
    gl_Position = matrix * vec4(V_pos, 1.f);
}

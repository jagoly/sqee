// GLSL Vertex Shader

layout(location=0) in vec3 V_pos;

uniform mat4 matrix;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    gl_Position = matrix * vec4(V_pos, 1.f);
}

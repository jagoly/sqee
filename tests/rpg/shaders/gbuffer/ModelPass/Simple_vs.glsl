// GLSL Vertex Shader

layout(location=0) in vec3 V_pos;
layout(location=1) in vec2 V_texcrd;
layout(location=2) in vec3 V_norm;
layout(location=3) in vec4 V_tan;

uniform mat4 matrix;
uniform mat3 normMat;

out vec2 texcrd;
out vec3 N, T, B;

out gl_PerVertex {
    vec4 gl_Position;
};


void main() {
    texcrd = V_texcrd;
    N = normalize(normMat * V_norm);
    T = normalize(normMat * V_tan.xyz);
    B = normalize(normMat * cross(V_norm, V_tan.xyz) * V_tan.w);
    gl_Position = matrix * vec4(V_pos, 1.f);
}

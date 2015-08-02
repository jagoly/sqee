// GLSL Vertex Shader

layout(location=0) in vec4 V_datA;
layout(location=1) in vec4 V_datB;

out float V_scale, V_alpha;
out vec3 V_pos, V_col;


void main() {
    V_scale = V_datA.w; 
    V_alpha = V_datB.a;
    V_pos = V_datA.xyz;
    V_col = V_datB.rgb;
}

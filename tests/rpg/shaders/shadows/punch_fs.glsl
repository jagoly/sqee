// GLSL Fragment Shader

in vec2 texcrd;

layout(binding=0) uniform sampler2D texDiff;


void main() {
    if (texture(texDiff, texcrd).a < 0.5f) discard;
}

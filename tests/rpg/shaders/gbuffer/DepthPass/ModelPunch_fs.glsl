// GLSL Fragment Shader

in vec2 texcrd;

layout(binding=0) uniform sampler2D texMask;

void main() {
    if (texture(texMask, texcrd).a < 0.5f) discard;
}

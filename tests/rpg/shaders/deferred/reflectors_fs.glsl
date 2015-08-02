// GLSL Fragment Shader

in vec2 texcrd;
in vec3 v_norm, v_from;
in noperspective vec2 s_tcrd;

#include builtin/blocks/camera
#include headers/blocks/reflector

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=2) uniform REFLECTORBLOCK { ReflectorBlock RB; };

layout(binding=0) uniform sampler2D texRefl;
layout(binding=2) uniform sampler2D texSpec;

out vec4 fragColour;


void main() {
    float fres = clamp(1.f - dot(v_norm, v_from), 0, 1);
    float factor = texture(texSpec, texcrd).r * RB.factor;
    fragColour = vec4(texture(texRefl, s_tcrd).rgb, 1.f);
    fragColour *= fres * factor;
}

// GLSL Fragment Shader

// define PIXSIZE vec2

in vec2 texcrd;
in vec3 v_norm, v_from;
in noperspective vec2 s_tcrd;

#include builtin/blocks/camera
#include headers/blocks/reflect

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=2) uniform REFLECTBLOCK { ReflectBlock RB; };

layout(binding=0) uniform sampler2D texRefl;
layout(binding=2) uniform sampler2D texSpec;

out vec4 fragColour;


void main() {
    vec2 offsU = vec2(s_tcrd.x, s_tcrd.y + 3.5f / 540.f);
    vec2 offsD = vec2(s_tcrd.x, s_tcrd.y - 3.5f / 540.f);
    vec2 offsR = vec2(s_tcrd.x + 3.5f / 960.f, s_tcrd.y);
    vec2 offsL = vec2(s_tcrd.x - 3.5f / 960.f, s_tcrd.y);
    vec2 offsUR = vec2(s_tcrd.x + 2.f / 960.f, s_tcrd.y + 2.f / 540.f);
    vec2 offsUL = vec2(s_tcrd.x - 2.f / 960.f, s_tcrd.y + 2.f / 540.f);
    vec2 offsDR = vec2(s_tcrd.x + 2.f / 960.f, s_tcrd.y - 2.f / 540.f);
    vec2 offsDL = vec2(s_tcrd.x - 2.f / 960.f, s_tcrd.y - 2.f / 540.f);
    float fres = clamp(1.f - dot(v_norm, v_from), 0.f, 1.f);
    float factor = texture(texSpec, texcrd).r * RB.factor;

    fragColour.rgb = texture(texRefl, s_tcrd).rgb;
    fragColour.rgb += texture(texRefl, s_tcrd + vec2(+1.5f, 0.f) * PIXSIZE).rgb;
    fragColour.rgb += texture(texRefl, s_tcrd + vec2(-1.5f, 0.f) * PIXSIZE).rgb;
    fragColour.rgb += texture(texRefl, s_tcrd + vec2(0.f, +1.5f) * PIXSIZE).rgb;
    fragColour.rgb += texture(texRefl, s_tcrd + vec2(0.f, -1.5f) * PIXSIZE).rgb;
    fragColour.rgb += texture(texRefl, s_tcrd + vec2(+1.f, +1.f) * PIXSIZE).rgb;
    fragColour.rgb += texture(texRefl, s_tcrd + vec2(+1.f, -1.f) * PIXSIZE).rgb;
    fragColour.rgb += texture(texRefl, s_tcrd + vec2(-1.f, +1.f) * PIXSIZE).rgb;
    fragColour.rgb += texture(texRefl, s_tcrd + vec2(-1.f, -1.f) * PIXSIZE).rgb;
    fragColour.rgb /= 9.f; fragColour.a = 1.f;

    fragColour *= fres * factor;
}

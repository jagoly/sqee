// GLSL Fragment Shader

in vec3 cubeNorm;

#include builtin/funcs/colours
#include headers/blocks/skybox

layout(std140, binding=1) uniform SKYBOXBLOCK { SkyBoxBlock SB; };

layout(binding=0) uniform samplerCube texSkyBox;

out vec4 fragColour;


vec3 curve_rgb_contrast(vec3 _c, float _contrast) {
    return vec3(_c - 0.5f) * _contrast + 0.5f;
}

void main() {
    vec4 texel = texture(texSkyBox, cubeNorm);
    vec3 hsl = rgb_to_hsl(texel.rgb);

    hsl.y *= SB.saturation; 
    texel.rgb = hsl_to_rgb(hsl);

    texel.rgb = curve_rgb_contrast(texel.rgb, SB.contrast);
    fragColour = vec4(texel.rgb + SB.brightness, texel.a * SB.alpha);
}

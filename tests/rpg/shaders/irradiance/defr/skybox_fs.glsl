// GLSL Fragment Shader

in vec3 cubeNorm;

#include builtin/funcs/colours
#include headers/blocks/skybox

layout(std140, binding=1) uniform SKYBOXBLOCK { SkyBoxBlock SB; };

layout(binding=0) uniform samplerCube texSkyBox;

out vec4 fragColour;


void main() {
    vec4 texel = texture(texSkyBox, cubeNorm);
    vec3 hsl = rgb_to_hsl(texel.rgb);

    hsl.y *= SB.saturation; 
    texel.rgb = hsl_to_rgb(hsl);

    texel.rgb = vec3(texel.rgb - 0.5f) * SB.contrast + 0.5f;
    fragColour = vec4(texel.rgb + SB.brightness, texel.a * SB.alpha);
}

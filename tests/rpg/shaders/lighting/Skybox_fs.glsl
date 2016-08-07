// GLSL Fragment Shader

in vec3 cubeNorm;

#include builtin/funcs/colours

uniform vec4 params;

layout(binding=0) uniform samplerCube texSkyBox;

out vec4 fragColour;


void main() {
    const float saturation = params.r;
    const float brightness = params.g;
    const float contrast = params.b;
    const float opacity = params.a;

    vec4 texel = texture(texSkyBox, cubeNorm);
    vec3 hsl = rgb_to_hsl(texel.rgb);

    hsl.y = hsl.y * saturation; 
    texel.rgb = hsl_to_rgb(hsl);

    texel.rgb = vec3(texel.rgb - 0.5f) * contrast + 0.5f;
    fragColour = vec4(texel.rgb + brightness, texel.a * opacity);
}

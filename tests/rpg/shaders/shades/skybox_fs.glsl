#version 330
#extension GL_ARB_shading_language_420pack : enable

in vec3 cubeNorm;

layout(binding=0) uniform samplerCube texSkyBox;

out vec4 fragColour;

void main() {
    fragColour = texture(texSkyBox, cubeNorm.xyz);
}

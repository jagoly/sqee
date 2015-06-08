#version 330
#extension GL_ARB_shading_language_420pack : enable

in vec3 cubeNorm;

#include "headers/blocks/skybox"

layout(std140, binding=1) uniform SKYBOXBLOCK { SkyBoxBlock SB; };

layout(binding=5) uniform samplerCube texSkyBox;

out vec4 fragColour;


void main() {
    fragColour = texture(texSkyBox, cubeNorm) * SB.colour;
}

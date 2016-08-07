// GLSL Fragment Shader

in vec2 texcrd;

#include headers/blocks/Camera
#include headers/blocks/Ambient

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform AMBIENTBLOCK { AmbientBlock LB; };

layout(binding=3) uniform sampler2D defrDiff;

out vec3 fragColour;


void main() {
    vec3 value = texture(defrDiff, texcrd).rgb;
    fragColour = value * LB.colour;
}

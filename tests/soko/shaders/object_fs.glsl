// GLSL Fragment Shader

in vec3 v_pos;
in vec3 v_norm;
in vec3 colour;

#include builtin/blocks/camera
#include uniform_block

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform UNIFORMBLOCK { UniformBlock UB; };

out vec4 fragColour;


void main() {
    vec3 lightDir = mat3(CB.view) * UB.slDirection;
    float dotProd = max(dot(-lightDir, v_norm), 0.f);
    fragColour.rgb = dotProd * UB.slColour * colour;
    fragColour.rgb += colour * UB.aColour;
}

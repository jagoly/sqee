// GLSL Fragment Shader

in vec3 v_pos;
in vec3 v_norm;
in vec3 colour;

#include uniform_block

layout(std140, binding=0) uniform UNIFORM { UniformBlock UB; };

out vec4 fragColour;


void main() 
{
    vec3 lightDir = mat3(UB.cameraView) * normalize(UB.lightDirection);
    float dotProd = max(dot(-lightDir, v_norm), 0.f);
    
    fragColour.rgb = colour * UB.ambientColour;
    fragColour.rgb += colour * UB.lightColour * dotProd;
}

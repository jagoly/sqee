// GLSL Uniform Block

layout(std140, set=0, binding=1) uniform LightBlock
{
    vec3 ambiColour;
    vec3 skyColour;
    vec3 skyDirection;
    mat4 skyMatrix;
}
LB;

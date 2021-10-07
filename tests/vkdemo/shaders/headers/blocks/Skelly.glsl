// GLSL Uniform Block

layout(std140, set=2, binding=0) uniform SkellyBlock
{
    mat4 matrix;
    mat3 normMat;
    mat3x4 bones[80];
}
SB;

// GLSL Uniform Block

struct LightOrthoBlock
{
    vec3 direction; // 12
    float density;  //  4
    vec3 colour;    // 16
    mat4 matrix;    // 64

    // TOTAL: 96
};

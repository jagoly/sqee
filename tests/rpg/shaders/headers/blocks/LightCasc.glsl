// GLSL Uniform Block

struct LightCascBlock
{
    vec3 direction;    //  12
    uint cascadeCount; //   4
    vec3 colour;       //  12
    float cascadeSize; //   4
    mat4 matrices[6];  // 384

    // TOTAL: 416
};

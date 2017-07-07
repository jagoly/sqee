// GLSL Uniform Block

struct LightPointBlock
{
    vec3 position;    //  12
    float intensity;  //   4
    vec3 colour;      //  16
    mat4 matrices[6]; // 384

    // TOTAL: 416
};

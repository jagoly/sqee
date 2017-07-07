// GLSL Uniform Block

struct LightSpotBlock
{
    vec3 position;   // 12
    float intensity; //  4
    vec3 direction;  // 12
    float softness;  //  4
    vec3 colour;     // 12
    float angle;     //  4
    mat4 matrix;     // 64

    // TOTAL: 112
};

// GLSL Uniform Block

struct SkyLightBlock {
    vec3 direction;  // 4
    vec3 colour;     // 3
    float density;   // 1
    mat4 matArrA[4]; // 64
    mat4 matArrB[2]; // 32
    vec4 splits;     // 4
    // Size: 108
};

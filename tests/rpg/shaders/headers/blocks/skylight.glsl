// GLSL Header File

struct SkyLightBlock {
    vec3 colour;     // 4
    vec3 direction;  // 4
    vec4 splitsA;    // 4
    vec2 splitsB;    // 4
    mat4 matArrA[4]; // 64
    mat4 matArrB[2]; // 32
    // Size: 112
};

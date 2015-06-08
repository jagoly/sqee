// GLSL Header File

struct SkyLightBlock {
    vec3 direction;  // 4
    vec3 colour;     // 4
    mat4 matArrA[4]; // 64
    mat4 matArrB[2]; // 32
    vec4 splits;     // 4
    // Size: 108
};

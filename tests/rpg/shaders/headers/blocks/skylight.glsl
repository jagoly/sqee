// GLSL Header File

struct SkyLightBlock {
    vec3 colour;    // 4
    vec3 direction; // 4
    vec4 splits;    // 4
    mat4 matArr[4]; // 64
    // Size: 76
};

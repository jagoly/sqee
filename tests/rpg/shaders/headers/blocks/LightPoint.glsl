// GLSL Uniform Block

struct LightPointBlock {
    vec3 position;    // 3
    float intensity;  // 1
    vec3 colour;      // 4
    mat4 matrices[6]; // 96
    // Size: 104
};

// GLSL Uniform Block

struct LightCascBlock {
    vec3 direction;    // 3
    uint cascadeCount; // 1
    vec3 colour;       // 3
    float cascadeSize; // 1
    mat4 matrices[6];  // 96
    // Size: 104
};

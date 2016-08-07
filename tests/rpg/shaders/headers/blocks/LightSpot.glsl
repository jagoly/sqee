// GLSL Uniform Block

struct LightSpotBlock {
    vec3 position;   // 3
    float intensity; // 1
    vec3 direction;  // 3
    float softness;  // 1
    vec3 colour;     // 3
    float angle;     // 1
    mat4 matrix;     // 16
    // Size: 28
};

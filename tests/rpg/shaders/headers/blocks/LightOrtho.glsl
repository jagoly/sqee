// GLSL Uniform Block

struct LightOrthoBlock {
    vec3 direction; // 3
    float density;  // 1
    vec3 colour;    // 4
    mat4 matrix;    // 16
    // Size: 24
};

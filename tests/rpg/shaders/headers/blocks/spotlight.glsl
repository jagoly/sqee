// GLSL Header File

struct SpotLightBlock {
    vec3 direction;  // 3
    float intensity; // 1
    vec3 position;   // 3
    float softness;  // 1
    vec3 colour;     // 3
    float angle;     // 1
    mat4 matrix;     // 16
    mat4 modelMat;   // 16
    // Size: 44
};

// GLSL Header File

struct PointLightBlock {
    vec3 position;   // 4
    vec3 colour;     // 3
    float intensity; // 1
    mat4 matArr[6];  // 96
    // Size: 104
};

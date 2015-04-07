// GLSL Header File

struct CameraBlock {
    mat4 proj;   // 16
    mat4 view;   // 16
    vec3 pos;    // 3
    float near;  // 1
    vec3 rot;    // 3
    float far;   // 1
    vec4 splits; // 4
    // Size: 46
};

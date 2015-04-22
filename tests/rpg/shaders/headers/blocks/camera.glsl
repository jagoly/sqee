// GLSL Header File

struct CameraBlock {
    mat4 proj;    // 16
    mat4 view;    // 16
    mat4 invProj; // 16
    mat4 invView; // 16
    mat4 trnView; // 16
    vec3 pos;     // 3
    float near;   // 1
    vec3 dir;     // 3
    float far;    // 1
    // Size: 90
};

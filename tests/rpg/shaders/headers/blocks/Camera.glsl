// GLSL Uniform Block

struct CameraBlock {
    mat4 view;    // 16
    mat4 proj;    // 16
    mat4 invView; // 16
    mat4 invProj; // 16
    mat4 trnView; // 16
    vec3 pos;     // 4
    vec3 dir;     // 4
    // Size: 88
};

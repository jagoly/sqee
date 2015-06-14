// GLSL Header File

struct MSkellyBlock {
    mat4 matrix;      // 16
    mat4 normMat;     // 16
    mat3x4 bones[80]; // 960
    // Size: 992
};

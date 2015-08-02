// GLSL Uniform Block

struct ReflectorBlock {
    mat4 matrix;  // 16
    mat4 normMat; // 16
    vec3 normal;  // 4
    vec3 trans;   // 3
    float factor; // 1
    // Size: 40
};

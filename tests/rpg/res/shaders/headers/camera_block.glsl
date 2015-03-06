// GLSL Header File

struct CSMStuff {
    float sa0, sa1, sa2, sa3, sa4, sa5; // 6
    float sb0, sb1, sb2, sb3;           // 4
    uint cntA, cntB;                    // 2
};

struct CameraBlock {
    mat4 proj;       // 16
    mat4 view;       // 16
    vec3 pos;        // 3
    float near;      // 1
    vec3 rot;        // 3
    float far;       // 1
    CSMStuff csm;    // 12
    // Size: 54
};

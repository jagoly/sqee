// GLSL Header File

struct SpotLight {
    vec3 pos;        // 3
    float angle;     // 1
    vec3 dir;        // 3
    float intensity; // 1
    vec3 colour;     // 3
    float softness;  // 1
    mat4 matArr[4];  // 64
    // Size: 76
};

struct WorldBlock {
    vec3 ambiColour;      // 3
    bool skylEnable;      // 1
    vec3 skylDir;         // 4
    vec3 skylColour;      // 4
    mat4 skylMatArr[6];   // 96
    uint spotCount;       // 4
    SpotLight spotArr[8]; // 608
    // Size: 720
};

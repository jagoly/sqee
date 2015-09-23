// GLSL Uniform Block

struct LightProbeBlock {
    mat4 viewMatArr[6]; // 96
    mat4 invViewArr[6]; // 96
    mat4 trnViewArr[6]; // 96
    // Size: 288
};

const mat4 LP_invProj = mat4(
    1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 0.f, -24.995f,
    0.f, 0.f, -1.f, 25.005f
);

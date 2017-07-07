// GLSL Header File

struct UniformBlock
{
    mat4 cameraView;     // 64
    mat4 cameraProj;     // 64
    vec3 lightDirection; // 16
    vec3 lightColour;    // 16
    vec3 ambientColour;  // 16
    
    // TOTAL: 176
};

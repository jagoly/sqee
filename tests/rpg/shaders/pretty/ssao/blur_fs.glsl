// GLSL Fragment Shader

// define HIGH
// define PIXSIZE vec2

in vec2 texcrd;

layout(binding=0) uniform sampler2D texAmbOcc;

out float fragColour;


const vec3 offsets[20] = {
vec3(-1.f, -1.f, 0.05f),
vec3(-1.f,  0.f, 0.06f),
vec3(-1.f, +1.f, 0.05f),
vec3( 0.f, -1.f, 0.06f),
vec3( 0.f, +1.f, 0.06f),
vec3(+1.f, -1.f, 0.05f),
vec3(+1.f,  0.f, 0.06f),
vec3(+1.f, +1.f, 0.05f),

vec3(-2.f, -1.f, 0.04f),
vec3(-2.f,  0.f, 0.05f),
vec3(-2.f, +1.f, 0.04f),

vec3(-1.f, -2.f, 0.04f),
vec3( 0.f, -2.f, 0.05f),
vec3(+1.f, -2.f, 0.04f),

vec3(+2.f, -1.f, 0.04f),
vec3(+2.f,  0.f, 0.05f),
vec3(+2.f, +1.f, 0.04f),

vec3(-1.f, +2.f, 0.04f),
vec3( 0.f, +2.f, 0.05f),
vec3(+1.f, +2.f, 0.04f),
};


void main() {
    float centreTexel = texture(texAmbOcc, texcrd).r;
    float vecLength = length(vec3(1.f, 1.f, 1.f));
    
    float texelA = texture(texAmbOcc, texcrd + offsets[0].xy * PIXSIZE).r;
    float texelB = texture(texAmbOcc, texcrd + offsets[1].xy * PIXSIZE).r;
    float texelC = texture(texAmbOcc, texcrd + offsets[2].xy * PIXSIZE).r;
    float texelD = texture(texAmbOcc, texcrd + offsets[3].xy * PIXSIZE).r;
    float texelE = texture(texAmbOcc, texcrd + offsets[4].xy * PIXSIZE).r;
    float texelF = texture(texAmbOcc, texcrd + offsets[5].xy * PIXSIZE).r;
    float texelG = texture(texAmbOcc, texcrd + offsets[6].xy * PIXSIZE).r;
    float texelH = texture(texAmbOcc, texcrd + offsets[7].xy * PIXSIZE).r;

    float weightA = distance(texelA, centreTexel) / vecLength * offsets[0].z;
    float weightB = distance(texelB, centreTexel) / vecLength * offsets[1].z;
    float weightC = distance(texelC, centreTexel) / vecLength * offsets[2].z;
    float weightD = distance(texelD, centreTexel) / vecLength * offsets[3].z;
    float weightE = distance(texelE, centreTexel) / vecLength * offsets[4].z;
    float weightF = distance(texelF, centreTexel) / vecLength * offsets[5].z;
    float weightG = distance(texelG, centreTexel) / vecLength * offsets[6].z;
    float weightH = distance(texelH, centreTexel) / vecLength * offsets[7].z;

    float value = centreTexel * 0.04f;
    value += texelA * weightA + texelB * weightB;
    value += texelC * weightC + texelD * weightD;
    value += texelE * weightE + texelF * weightF;
    value += texelG * weightG + texelH * weightH;
    
    float normWeight = 0.04f;
    normWeight += weightA + weightB;
    normWeight += weightC + weightD;
    normWeight += weightE + weightF;
    normWeight += weightG + weightH;

    fragColour = value / normWeight;
}

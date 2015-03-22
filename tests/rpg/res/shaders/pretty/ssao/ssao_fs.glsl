#version 330
#extension GL_ARB_shading_language_420pack : enable

// define HIGH

in vec2 texcrd;

uniform vec2 pixSize;
uniform mat4 invProjMat;
layout(binding=1) uniform sampler2D texSurf;
layout(binding=5) uniform sampler2D texDepth;

out float fragColour;


#ifdef HIGH
const float distThres = 0.7f;
const float filterSize = 110.f;
const int diskSize = 24;
const vec2 disk[24] = {
vec2( 0.000000,  0.041667),
vec2(-0.021568, -0.080494),
vec2( 0.062500,  0.108253),
vec2(-0.117851, -0.117851),
vec2( 0.180422,  0.104167),
vec2(-0.241481, -0.064705),
vec2( 0.291667, -0.000000),
vec2(-0.321975,  0.086273),
vec2( 0.324760, -0.187500),
vec2(-0.294628,  0.294628),
vec2( 0.229167, -0.396928),
vec2(-0.129410,  0.482963),
vec2(-0.000000, -0.541667),
vec2( 0.150978,  0.563457),
vec2(-0.312500, -0.541266),
vec2( 0.471405,  0.471405),
vec2(-0.613435, -0.354167),
vec2( 0.724444,  0.194114),
vec2(-0.791667,  0.000000),
vec2( 0.804938, -0.215683),
vec2(-0.757772,  0.437500),
vec2( 0.648181, -0.648181),
vec2(-0.479167,  0.829941),
vec2( 0.258819, -0.965926),
};
#else
const float distThres = 0.5f;
const float filterSize = 50.f;
const int diskSize = 10;
const vec2 disk[10] = {
vec2(0.000000, 0.100000),
vec2(-0.117557, -0.161803),
vec2(0.285317, 0.092705),
vec2(-0.380423, 0.123607),
vec2(0.293893, -0.404508),
vec2(0.000000, 0.600000),
vec2(-0.411450, -0.566312),
vec2(0.760845, 0.247214),
vec2(-0.855951, 0.278115),
vec2(0.587785, -0.809017),
};
#endif

vec3 get_view_pos(in vec2 _tc) {
    float dep = texture(texDepth, _tc).r * 2.f - 1.f;
    vec4 p_pos = vec4(_tc * 2.f - 1.f, dep, 1.f);
    vec4 v_pos = invProjMat * p_pos;
    return v_pos.xyz / v_pos.w;
}

float rand(vec2 _crd){
    return fract(sin(dot(_crd, vec2(12.9898f,78.233f))) * 43758.5453);
}

void main() {
    vec3 pos = get_view_pos(texcrd);
    vec2 fsvspz = filterSize * pixSize / pos.z;
    vec3 v_surf = normalize(texture(texSurf, texcrd).rgb * 2.f - 1.f);

    float angle = rand(texcrd.xy);
    float s = sin(angle);
    float c = cos(angle);

    float ambiOcc = 0.f;
    for (int i = 0; i < diskSize; i++) {
        vec2 offs = disk[i];
        offs = vec2(c * offs.x - s * offs.y, c * offs.y + s * offs.x);
        vec3 sampPos = get_view_pos(texcrd + offs * fsvspz);
        vec3 sampDir = normalize(sampPos - pos);
        float dist = distance(pos, sampPos);

        float a = max(dot(v_surf, sampDir), 0.f);
        float b = 1.f - smoothstep(distThres, distThres * 2.f, dist);
        ambiOcc += a * b;
    }

    fragColour = 1.f - ambiOcc / float(diskSize);
}
